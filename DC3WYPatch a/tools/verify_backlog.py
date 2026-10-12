"""Execute the actual x86 DLL hook and original game's name lookup in Unicorn.

Usage: python verify_backlog.py GAME.EXE PATCH.dll NAME_AUDIT.json
Requires pefile, capstone, unicorn. No game files are modified.
"""
import json
import re
import struct
import sys
from pathlib import Path

import capstone
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *

game, dll = [pefile.PE(p) for p in sys.argv[1:3]]
audit = json.loads(Path(sys.argv[3]).read_text(encoding='utf-8-sig'))
assert not audit['length_mismatches']
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
image = dll.get_memory_mapped_image()
prefix = bytes.fromhex('9c608d0c248d5424348d442460')
assert image.count(prefix) == 1, 'Compiled trampoline signature changed'
hook = dll.OPTIONAL_HEADER.ImageBase + image.index(prefix)
insns = list(md.disasm(image[image.index(prefix):image.index(prefix)+64], hook))
resolver = int(next(i.op_str for i in insns if i.mnemonic == 'call'), 16)
resolver_insns = list(md.disasm(image[resolver-dll.OPTIONAL_HEADER.ImageBase:
                                       resolver-dll.OPTIONAL_HEADER.ImageBase+32], resolver))
setting_cmp = next(i for i in resolver_insns if i.mnemonic == 'cmp' and 'byte ptr [0x' in i.op_str)
setting = int(re.search(r'0x[0-9a-f]+', setting_cmp.op_str)[0], 16)
stack = 0x20008000
regs = [UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
        UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]

def make_vm():
    u = Uc(UC_ARCH_X86, UC_MODE_32)
    for p in [game, dll]:
        base = p.OPTIONAL_HEADER.ImageBase
        u.mem_map(base, (p.OPTIONAL_HEADER.SizeOfImage+4095) & ~4095)
        u.mem_write(base, p.get_memory_mapped_image())
    u.mem_map(0x20000000, 0x10000)
    return u

u = make_vm()
def dword(addr):
    return struct.unpack('<I', u.mem_read(addr, 4))[0]

def cstring(addr):
    data = bytearray()
    for i in range(2048):
        b = u.mem_read(addr+i, 1)[0]
        if b == 0:
            return bytes(data)
        data.append(b)
    raise AssertionError('Unterminated string')

def intercept(u, addr, size, _):
    if addr in (0x404BFE, 0x404C04, 0x404D2E):
        u.emu_stop()
    elif addr == 0x448990: # cdecl strstr; all other lookup code executes unchanged.
        esp = u.reg_read(UC_X86_REG_ESP)
        haystack, needle = dword(esp+4), dword(esp+8)
        offset = cstring(haystack).find(cstring(needle))
        u.reg_write(UC_X86_REG_EAX, haystack+offset if offset >= 0 else 0)
        u.reg_write(UC_X86_REG_EIP, dword(esp))
        u.reg_write(UC_X86_REG_ESP, esp+4)
u.hook_add(UC_HOOK_CODE, intercept)

def reset(name):
    u.mem_write(stack-256, bytes(1024))
    u.mem_write(stack+0x3c, name+b'\0')
    for i, reg in enumerate(regs):
        u.reg_write(reg, 0x123400+i*0x100)
    u.reg_write(UC_X86_REG_ESP, stack)
    u.reg_write(UC_X86_REG_EFLAGS, 0x246)

def original(name):
    reset(name.encode('cp932'))
    u.reg_write(UC_X86_REG_EAX, 0)
    u.reg_write(UC_X86_REG_EDX, stack+0x3c)
    u.emu_start(0x404680, 0, count=10000)
    assert u.reg_read(UC_X86_REG_EIP) in (0x404BFE, 0x404D2E)
    if u.reg_read(UC_X86_REG_EIP) == 0x404BFE:
        return None
    return dword(stack+0x10), u.reg_read(UC_X86_REG_EDI)

def translated(name, expected, enabled=True):
    reset(name)
    u.mem_write(setting, bytes([enabled]))
    initial = {r:u.reg_read(r) for r in regs}
    before = bytes(u.mem_read(stack, 256))
    u.emu_start(hook, 0, count=10000)
    assert u.reg_read(UC_X86_REG_ESP) == stack, 'Stack imbalance'
    assert u.reg_read(UC_X86_REG_EFLAGS) == 0x246, 'Flags clobbered'
    if expected is not None and enabled:
        assert u.reg_read(UC_X86_REG_EIP) == 0x404D2E, name
        assert (dword(stack+0x10),u.reg_read(UC_X86_REG_EDI)) == expected, name
        initial[UC_X86_REG_EDI] = expected[1]
        wanted = bytearray(before)
        wanted[0x10:0x14] = struct.pack('<I',expected[0])
        assert bytes(u.mem_read(stack,256)) == wanted, 'Unexpected stack write'
    else:
        assert u.reg_read(UC_X86_REG_EIP) == 0x404C04, name
        initial[UC_X86_REG_EDX] = (initial[UC_X86_REG_EDX]&~255) | u.mem_read(0x4795BA,1)[0]
        assert bytes(u.mem_read(stack,256)) == before, 'Fallback altered name/stack'
    assert all(u.reg_read(r)==v for r,v in initial.items()), 'Register clobbered'

supported = {}
for name, jp_names in audit['names'].items():
    expected = {original(jp) for jp in jp_names}
    assert len(expected) == 1, (name, expected)
    expected = expected.pop()
    for encoded in {name.encode('cp932'),name.replace(' ','_').encode('cp932')}:
        translated(encoded, expected)
        translated(encoded, expected, enabled=False)
    if expected is not None:
        supported[name] = expected
translated(b'',None)
translated(b'X'*32,None)
translated(b'Charles and Himeno',None)
translated(b'CharlesExtra',None)
for jp in ['シャルル','清隆','リッカ']:
    translated(jp.encode('cp932'),None) # Already handled before fallback in game.
print(f'PASS: {len(audit["names"])} JSON names, {len(supported)} supported aliases; '
      'actual DLL machine code matches original atlas lookup. '
      'Settings on/off, underscores, empty/unknown/group names, registers, '
      'stack and flags verified.')
print(json.dumps(supported,ensure_ascii=False,sort_keys=True))
