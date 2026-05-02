cmake_minimum_required(VERSION 3.15)
project(DC3WYPatch)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Error: Building for x64, but x86 expected. Use x86 Native Tools Command Prompt.")
endif()

option(BUILD_DC4PH "Build for Da Capo 4 Plus Harmony" OFF)
option(BUILD_DC3WY "Build for Da Capo III With You" OFF)

if(BUILD_DC4PH AND BUILD_DC3WY)
    message(FATAL_ERROR "Error: BUILD_DC4PH and BUILD_DC3WY cannot both be ON.")
endif()

if(BUILD_DC4PH)
    message(STATUS "Configuring build for Da Capo 4 Plus Harmony...")
elseif(BUILD_DC3WY)
    message(STATUS "Configuring build for Da Capo III With You...")
endif()

# Detours library
set(DETOURS_DIR "${CMAKE_SOURCE_DIR}/Detours/src")
include_directories(${DETOURS_DIR})

add_library(detours STATIC
    ${DETOURS_DIR}/detours.cpp
    ${DETOURS_DIR}/modules.cpp
    ${DETOURS_DIR}/disasm.cpp
    ${DETOURS_DIR}/image.cpp
    ${DETOURS_DIR}/creatwth.cpp
    ${DETOURS_DIR}/disolx86.cpp
)

# ============================================================================
# DC4Patch.dll - The actual patch (injected by launcher)
# ============================================================================
add_library(DC4Patch SHARED
    src/dc4patch.cpp
)

target_link_libraries(DC4Patch PRIVATE
    detours
    gdi32
    user32
    advapi32
    comdlg32
    shell32
    ole32
    shlwapi
)

set_target_properties(DC4Patch PROPERTIES
    LINK_FLAGS "/DYNAMICBASE:NO"
)

if(BUILD_DC4PH)
    set_target_properties(DC4Patch PROPERTIES OUTPUT_NAME "DC4PHPatch")
    target_compile_definitions(DC4Patch PRIVATE GAME_DC4PH)
elseif(BUILD_DC3WY)
    set_target_properties(DC4Patch PROPERTIES OUTPUT_NAME "DC3WYPatch")
    target_compile_definitions(DC4Patch PRIVATE GAME_DC3WY)
else()
    set_target_properties(DC4Patch PROPERTIES OUTPUT_NAME "DC3PPPatch")
    target_compile_definitions(DC4Patch PRIVATE GAME_DC3PP)
endif()

# ============================================================================
# DC3WYLauncher.exe - Starts the game with built-in Locale Emulator + patch injection
# ============================================================================
add_executable(DC4Launcher WIN32
    src/launcher.cpp
    src/launcher.rc
    src/le_resources.rc
)

target_include_directories(DC4Launcher PRIVATE src)

target_link_libraries(DC4Launcher PRIVATE
    user32
    advapi32
    shell32
    shlwapi
    ole32
)

if(BUILD_DC4PH)
    set_target_properties(DC4Launcher PROPERTIES OUTPUT_NAME "DC4PHLauncher")
    target_compile_definitions(DC4Launcher PRIVATE GAME_DC4PH)
elseif(BUILD_DC3WY)
    set_target_properties(DC4Launcher PROPERTIES OUTPUT_NAME "DC3WYLauncher")
    target_compile_definitions(DC4Launcher PRIVATE GAME_DC3WY)
else()
    set_target_properties(DC4Launcher PROPERTIES OUTPUT_NAME "DC3PPLauncher")
    target_compile_definitions(DC4Launcher PRIVATE GAME_DC3PP)
endif()
