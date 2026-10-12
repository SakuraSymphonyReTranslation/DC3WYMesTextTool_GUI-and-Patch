cmake_minimum_required(VERSION 3.15)
project(DC3WYPatch LANGUAGES C CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 20)
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "DC3WY requires an x86 build. Configure with -A Win32.")
endif()
set(DETOURS_DIR "${CMAKE_SOURCE_DIR}/Detours/src")
add_library(detours STATIC
    ${DETOURS_DIR}/detours.cpp
    ${DETOURS_DIR}/modules.cpp
    ${DETOURS_DIR}/disasm.cpp
    ${DETOURS_DIR}/image.cpp
    ${DETOURS_DIR}/creatwth.cpp
    ${DETOURS_DIR}/disolx86.cpp
)
target_include_directories(detours PUBLIC ${DETOURS_DIR})
add_library(DC3WYPatch SHARED src/dc3wypatch.cpp)
target_link_libraries(DC3WYPatch PRIVATE
    detours gdi32 user32 advapi32 comdlg32 shell32 ole32 shlwapi
)
set_target_properties(DC3WYPatch PROPERTIES LINK_FLAGS "/DYNAMICBASE:NO")
add_executable(DC3WYLauncher WIN32
    src/launcher.cpp
    src/launcher.rc
    src/le_resources.rc
)
target_include_directories(DC3WYLauncher PRIVATE src)
target_link_libraries(DC3WYLauncher PRIVATE user32 advapi32 shell32 shlwapi ole32)
