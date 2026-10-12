"""Run the compiled x86 line-advance hook and helper against the game in Unicorn.

Usage: python verify_line_spacing.py GAME.EXE PATCH.dll
The real DLL's TLS spacing setting is populated; no helper code is mocked.
"""
import json
import re
import struct
import sys

import capstone
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *

game, dll = [pefile.PE(p) for p in sys.argv[1:3]]
image = dll.get_memory_mapped_image()
base = dll.OPTIONAL_HEADER.ImageBase
prefix = bytes.fromhex('8b8424a80000009c6050')
assert image.count(prefix) == 1
offset = image.index(prefix)
hook = base + offset
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
insns = list(md.disasm(image[offset:offset+32], hook))
helper = next(int(i.op_str, 16) for i in insns if i.mnemonic == 'call')
helper_insns = list(md.disasm(image[helper-base:helper-base+64], helper))
tls_index = next(int(re.search(r'0x[0-9a-f]+', i.op_str)[0], 16)
                 for i in helper_insns if i.mnemonic == 'mov' and i.op_str.startswith('ecx, dword ptr [0x'))
tls_offset = next(int(re.search(r'0x[0-9a-f]+', i.op_str)[0], 16)
                  for i in helper_insns if i.mnemonic == 'mov' and i.op_str.startswith('ecx, dword ptr [eax +'))
u = Uc(UC_ARCH_X86, UC_MODE_32)
for pe in [game, dll]:
    u.mem_map(pe.OPTIONAL_HEADER.ImageBase, (pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095)
    u.mem_write(pe.OPTIONAL_HEADER.ImageBase, pe.get_memory_mapped_image())
u.mem_map(0, 0x10000)
u.mem_map(0x20000000, 0x10000)

def put(addr, value):
    u.mem_write(addr, struct.pack('<I', value & 0xffffffff))

def get(addr):
    return struct.unpack('<I', u.mem_read(addr, 4))[0]

put(tls_index, 0)
put(0x2c, 0x1000)
put(0x1000, 0x2000)
stack = 0x20008000
regs = [UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
        UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]

def run(height, spacing, patched):
    u.mem_write(stack-256, bytes(768))
    put(stack+0xa8, height)
    put(stack+0x30, 99)
    put(0x2000+tls_offset, spacing)
    for i, reg in enumerate(regs): u.reg_write(reg, 0x123400+i*0x100)
    u.reg_write(UC_X86_REG_ESP, stack)
    u.reg_write(UC_X86_REG_EFLAGS, 0x246)
    before = bytes(u.mem_read(stack, 256))
    if patched:
        u.emu_start(hook, 0x435B30, count=1000)
        assert u.reg_read(UC_X86_REG_EFLAGS) == 0x246
        assert bytes(u.mem_read(stack, 256)) == before
        u.emu_start(0x435B30, 0x435B34, count=10)
    else:
        u.emu_start(0x435B29, 0x435B34, count=10)
    assert u.reg_read(UC_X86_REG_EIP) == 0x435B34
    assert u.reg_read(UC_X86_REG_ESP) == stack
    for i, reg in enumerate(regs[1:], 1):
        assert u.reg_read(reg) == 0x123400+i*0x100
    after = bytearray(before)
    advance = u.reg_read(UC_X86_REG_EAX)
    after[0x30:0x34] = struct.pack('<I', 99+advance)
    assert bytes(u.mem_read(stack, 256)) == after
    return get(stack+0x30)-99

cases = 0
for height in [0, 10, 11, 12, 24, 34, 44]:
    assert run(height, 0, False) == height
    for spacing in [-2147483648, -20, -10, -5, 0, 2, 64, 2147483647]:
        expected = height if height < 12 else height + max(-10, min(64, spacing))
        assert run(height, spacing, True) == expected
        cases += 1
assert run(34, -10, True) == 24
assert run(44, -10, True) == 34
print(json.dumps({'passed_cases': cases, 'normal_pitch': 24,
                  'four_line_offsets': [0, 24, 48, 72],
                  'large_font_pitch': 34, 'stack_and_registers_preserved': True,
                  'texture_height_preserved': True}))
