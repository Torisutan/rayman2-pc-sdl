# Modern toolchain build of Rayman2.exe (32-bit x86, current MSVC or clang-cl).
#
# MSVC (Visual Studio generator):
#   cmake -S . -B build-cmake -A Win32
#   cmake --build build-cmake --config Release
# clang-cl (from an x86 developer prompt, vcvars32.bat; any single-config generator):
#   cmake -S . -B build-clang -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
#         -DCMAKE_C_COMPILER=clang-cl -DCMAKE_C_COMPILER_TARGET=i686-pc-windows-msvc
#         -DCMAKE_LINKER=lld-link
#   cmake --build build-clang
#
# The translation units and their link order come from Rayman2.exe/tu.json, so TUs added upstream
# build without edits here. Paths to the non-shipped inputs come from local.json (see BUILDING.md):
#   vc6  Visual C++ 6.0, for DINPUT.LIB (DirectInputCreateA and the c_dfDI* data formats; current
#        SDKs ship only the DirectInput 8 library)
#
# SDL3 platform backend (R2_USE_SDL, off by default; docs/PLAN.md Phase 5): SDL3 comes from vcpkg
# in manifest mode (vcpkg.json, feature "sdl"), linked statically:
#   cmake -S . -B build-sdl -A Win32 -DR2_USE_SDL=ON -DVCPKG_TARGET_TRIPLET=x86-windows-static-md
#         -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required(VERSION 3.20)
option(R2_USE_SDL "Window, events and timers through SDL3 (USE_SDL; needs the vcpkg toolchain)" OFF)
if(R2_USE_SDL)
  # Read by the vcpkg toolchain at project(): installs the manifest's "sdl" feature.
  list(APPEND VCPKG_MANIFEST_FEATURES sdl)
endif()
project(Rayman2 C RC)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "Rayman2 builds as 32-bit x86 only: configure with -A Win32")
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
# Keep any build tree out of git, whatever its name.
file(WRITE ${CMAKE_BINARY_DIR}/.gitignore "*
")

set(R2_DIR ${CMAKE_SOURCE_DIR}/Rayman2.exe)

# ---- local.json ---------------------------------------------------------------------------------
set(R2_LOCAL ${CMAKE_SOURCE_DIR}/local.json)
if(NOT EXISTS ${R2_LOCAL})
  message(FATAL_ERROR "local.json missing: run `python tools/build.py --vc5 .. --vc6 .. --gog ..` once (BUILDING.md)")
endif()
file(READ ${R2_LOCAL} R2_LOCAL_JSON)
string(JSON R2_VC6 GET ${R2_LOCAL_JSON} vc6)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${R2_LOCAL} ${R2_DIR}/tu.json ${R2_DIR}/exports.txt)

# ---- translation units, in link order ---------------------------------------------------------
file(READ ${R2_DIR}/tu.json R2_TU_JSON)
string(JSON R2_TU_COUNT LENGTH ${R2_TU_JSON} link_order)
math(EXPR R2_TU_LAST "${R2_TU_COUNT} - 1")
set(R2_SOURCES)
foreach(i RANGE ${R2_TU_LAST})
  string(JSON src GET ${R2_TU_JSON} link_order ${i} src)
  list(APPEND R2_SOURCES ${R2_DIR}/${src})
  # Per-TU retail flags (G6 = Pentium Pro scheduling, MD = DLL CRT, none) only matter for byte
  # matching; the modern build uses one codegen for every TU.
endforeach()

# ---- resources: own icon and manifest (src/app/Rayman2.rc); the icon is generated ------------
set(R2_ICO ${CMAKE_BINARY_DIR}/Rayman2.ico)
add_custom_command(
  OUTPUT ${R2_ICO}
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/make_icon.py ${R2_ICO}
  DEPENDS ${CMAKE_SOURCE_DIR}/tools/make_icon.py
  COMMENT "Generating Rayman2.ico"
  VERBATIM)
set(R2_RC ${R2_DIR}/src/app/Rayman2.rc)
set_source_files_properties(${R2_RC} PROPERTIES OBJECT_DEPENDS "${R2_ICO};${R2_DIR}/src/app/Rayman2.manifest")

# ---- exports: the sound driver DLLs call back into these ---------------------------------------
file(STRINGS ${R2_DIR}/exports.txt R2_EXPORTS)
set(R2_EXPORT_FLAGS)
foreach(e ${R2_EXPORTS})
  string(STRIP "${e}" e)
  if(e)
    list(APPEND R2_EXPORT_FLAGS "/EXPORT:${e}")
  endif()
endforeach()

# ---- fork-only code (Rayman2.exe/src/app), each part behind its own define -----------------------
option(R2_REGRESS "Compile in the regression harness (src/app/regress.c; inactive unless R2_REGRESS_OUT is set)" ON)
set(R2_FORK_DEFINES)
if(R2_REGRESS)
  list(APPEND R2_SOURCES ${R2_DIR}/src/app/regress.c)
  list(APPEND R2_FORK_DEFINES RAYMAN2_REGRESS)
endif()
option(R2_NO_PROTECTION "Exclude the copy protection (RAYMAN2_NO_PROTECTION, docs/PLAN.md Phase 2)" ON)
if(R2_NO_PROTECTION)
  list(APPEND R2_FORK_DEFINES RAYMAN2_NO_PROTECTION)
endif()
option(R2_WIDESCREEN "Hor+ 3D view and 4:3 UI at wide modes (RAYMAN2_WIDESCREEN, docs/PLAN.md Display)" ON)
if(R2_WIDESCREEN)
  list(APPEND R2_SOURCES ${R2_DIR}/src/GLI/GLI_ratio.c)
  list(APPEND R2_FORK_DEFINES RAYMAN2_WIDESCREEN)
endif()
option(R2_FAST_LOADING "Refresh the loading screen progress bar at a bounded rate (RAYMAN2_FAST_LOADING)" ON)
if(R2_FAST_LOADING)
  list(APPEND R2_SOURCES ${R2_DIR}/src/GLI/GLI_fastload.c)
  list(APPEND R2_FORK_DEFINES RAYMAN2_FAST_LOADING)
endif()
option(R2_LOCAL_UBIINI "Read ubi.ini from the working directory, not the Windows directory (RAYMAN2_LOCAL_UBIINI)" ON)
if(R2_LOCAL_UBIINI)
  list(APPEND R2_FORK_DEFINES RAYMAN2_LOCAL_UBIINI)
endif()
option(R2_FIX_BUGS "Fix the original engine's crashes and undefined behaviour (RAYMAN2_FIX_BUGS, docs/BUGS.md)" ON)
if(R2_FIX_BUGS)
  list(APPEND R2_FORK_DEFINES RAYMAN2_FIX_BUGS)
endif()
# C runtime branches beside the Win32 ones: level data files (MMG/SNA_file.c), ini files
# (FIL/FIL_ini.c), the temp folder (FIL/FIL_path.c), memory (MMG/PrivMmg.h). Windows keeps the Win32 API (FILE_MODE_C_WIN95); this
# option takes the C runtime there too, to check it against the regression run (docs/PLAN.md
# Phase 5, step 3).
option(R2_C_RUNTIME "C runtime instead of the Win32 API for files, ini files and memory (RAYMAN2_C_RUNTIME)" OFF)
list(APPEND R2_SOURCES ${R2_DIR}/src/MMG/SNA_file.c ${R2_DIR}/src/FIL/FIL_ini.c ${R2_DIR}/src/FIL/FIL_path.c)
# The C branch of the MTH matrix product (MTH/MTH_Debug.c; MTH_C_DEV targets, docs/PLAN.md Phase 4).
list(APPEND R2_SOURCES ${R2_DIR}/src/MTH/MTH_Debug.c)
if(R2_C_RUNTIME)
  list(APPEND R2_FORK_DEFINES RAYMAN2_C_RUNTIME)
endif()
# The Direct3D 6 renderer driver is built from GliDX6vf.dll/ (its tu.json, in link order) and
# linked in; ubi.ini's GLI_DllFile=GliDX6 then selects it instead of DLL/GliDX6vf.dll. Other
# drivers are still loaded from the DLL folder (GLI/GLI_linkdrv.c, DEVIATIONS 13).
option(R2_GLI_DX6 "Link the Direct3D 6 renderer driver into the exe (USE_GLI_DX6)" ON)
if(R2_GLI_DX6)
  set(R2_GLI_DIR ${CMAKE_SOURCE_DIR}/GliDX6vf.dll)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${R2_GLI_DIR}/tu.json)
  file(READ ${R2_GLI_DIR}/tu.json R2_GLI_TU_JSON)
  string(JSON R2_GLI_TU_COUNT LENGTH ${R2_GLI_TU_JSON} link_order)
  math(EXPR R2_GLI_TU_LAST "${R2_GLI_TU_COUNT} - 1")
  set(R2_GLI_SOURCES)
  foreach(i RANGE ${R2_GLI_TU_LAST})
    string(JSON src GET ${R2_GLI_TU_JSON} link_order ${i} src)
    list(APPEND R2_GLI_SOURCES ${R2_GLI_DIR}/src/${src})
  endforeach()
  add_library(GliDX6vf STATIC ${R2_GLI_SOURCES})
  set_target_properties(GliDX6vf PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
  # tu.json's include list (relative to the repo root, in search order): the driver's own headers,
  # then the exe's (GLI/GLI_BigGlobals.h, shared with the engine).
  string(JSON R2_GLI_INC_COUNT LENGTH ${R2_GLI_TU_JSON} include)
  math(EXPR R2_GLI_INC_LAST "${R2_GLI_INC_COUNT} - 1")
  foreach(i RANGE ${R2_GLI_INC_LAST})
    string(JSON inc GET ${R2_GLI_TU_JSON} include ${i})
    target_include_directories(GliDX6vf PRIVATE ${CMAKE_SOURCE_DIR}/${inc})
  endforeach()
  target_include_directories(GliDX6vf INTERFACE ${R2_GLI_DIR}/include)
  # inline=__inline: as tu.json's base_flags and the Rayman2 target (the engine headers write inline).
  target_compile_definitions(GliDX6vf PRIVATE inline=__inline)
  target_compile_options(GliDX6vf PRIVATE $<$<NOT:$<CONFIG:Debug>>:/O2 /Gy>)
  if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Same policy as for Rayman2 below.
    target_compile_options(GliDX6vf PRIVATE
      -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration -Wno-error=int-conversion)
  endif()
  list(APPEND R2_SOURCES ${R2_DIR}/src/GLI/GLI_linkdrv.c)
  list(APPEND R2_FORK_DEFINES USE_GLI_DX6)
endif()
# The sound drivers WAVMWBVR (samples, mixer, waveOut) and APMmxBVR (ADPCM streams) built from
# their folders (tu.json, in link order, without the DLL glue snddll.c) and linked in, the original
# engine's static form: without _DLL_COMPILATION_MODE the exe's SND calls them directly and loads no
# driver DLL. The retail DLL/Snd_Cpa.ini binds only these two (DLL_Sxd, DLL_Adpcm); MPEG, Midi, CD
# and record have no driver section, so retail runs them on the no-module defaults. The original
# engine's DISABLE_* switches drop those modules; their call sites give the same values.
option(R2_SND_LINKED "Link the sound drivers into the exe (no _DLL_COMPILATION_MODE)" OFF)
if(R2_SND_LINKED)
  set(R2_SND_DISABLE DISABLE_MPEG DISABLE_MIDI DISABLE_CD DISABLE_RECORD)
  list(APPEND R2_FORK_DEFINES ${R2_SND_DISABLE})
  # The sample driver's mixer: the C branch (src/mixer/sndmix_pc.c) by default, on every target. The MMX
  # asm (src/mixer/mix*.asm, OPTIMIZED_FOR_MMX_WITH_ASM) only on request and only on 32-bit x86 with MSVC
  # or clang-cl, for comparison: it relies on the retail DLL's .bss layout, which this link does not
  # reproduce (PLAN Sound item 14).
  option(R2_SND_MIXER_ASM "Sound mixer from the MMX asm instead of sndmix_pc.c (32-bit x86 MSVC only)" OFF)
  if(R2_SND_MIXER_ASM AND NOT (MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4))
    message(FATAL_ERROR "R2_SND_MIXER_ASM needs a 32-bit x86 MSVC or clang-cl build")
  endif()
  enable_language(ASM_MASM)
  foreach(drv WAVMWBVR APMmxBVR)
    set(dir ${CMAKE_SOURCE_DIR}/${drv}.dll)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${dir}/tu.json)
    file(READ ${dir}/tu.json json)
    string(JSON n LENGTH ${json} link_order)
    math(EXPR last "${n} - 1")
    set(srcs)
    foreach(i RANGE ${last})
      string(JSON src GET ${json} link_order ${i} src)
      if(NOT src MATCHES "(^|/)snddll\.c$|\.rc$" AND NOT (R2_USE_SDL AND src STREQUAL "pc_mixervol.c")
         AND NOT (src MATCHES "^mixer/.*\.asm$" AND NOT R2_SND_MIXER_ASM))
        list(APPEND srcs ${dir}/src/${src})
      endif()
    endforeach()
    if(drv STREQUAL "WAVMWBVR")
      list(APPEND srcs ${dir}/src/mixer/sndmix_pc.c)
    endif()
    if(R2_USE_SDL AND drv STREQUAL "WAVMWBVR")
      # Output and volume through SDL instead of waveOut and the system mixer (pc_mixervol.c).
      list(APPEND srcs ${dir}/src/sdl_sndwave.c)
    endif()
    add_library(${drv} STATIC ${srcs})
    set_target_properties(${drv} PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
    target_include_directories(${drv} PRIVATE ${dir}/include ${CMAKE_SOURCE_DIR}/WAVMWBVR.dll/include)
    target_compile_definitions(${drv} PRIVATE VISUAL ${R2_SND_DISABLE} _CRT_SECURE_NO_WARNINGS
      $<$<BOOL:${R2_USE_SDL}>:USE_SDL> $<$<BOOL:${R2_SND_MIXER_ASM}>:OPTIMIZED_FOR_MMX_WITH_ASM>)
    target_compile_options(${drv} PRIVATE $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<CONFIG:Debug>>>:/O2 /Gy>)
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
      target_compile_options(${drv} PRIVATE $<$<COMPILE_LANGUAGE:C>:
        -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration -Wno-error=int-conversion>)
    endif()
  endforeach()
else()
  list(APPEND R2_FORK_DEFINES _DLL_COMPILATION_MODE)
endif()
# USE_SDL also leaves FILE_MODE_C_WIN95 unset (SND/snddef.h): the C runtime branches come with it.
if(R2_USE_SDL)
  find_package(SDL3 CONFIG REQUIRED)
  # Window, events and engine loop on the main thread, beside GAM/pc_winmain.c's Win32 parts;
  # timer counters beside the PC ones (GLI/cpu_timer.c, GLI/timer_sub2.c); input devices beside
  # INO/INO.c's DirectInput parts; the error manager's message box beside ERM/CpaError.c's.
  list(APPEND R2_SOURCES ${R2_DIR}/src/GAM/sdl_main.c)
  list(APPEND R2_SOURCES ${R2_DIR}/src/TMR/sdl_timer.c)
  list(APPEND R2_SOURCES ${R2_DIR}/src/INO/sdl_ino.c)
  list(APPEND R2_SOURCES ${R2_DIR}/src/MMG/sdl_mmg.c)
  list(APPEND R2_SOURCES ${R2_DIR}/src/ERM/sdl_erm.c)
  list(APPEND R2_FORK_DEFINES USE_SDL)
endif()

add_executable(Rayman2 WIN32 ${R2_SOURCES} ${R2_RC} ${R2_ICO})
set_target_properties(Rayman2 PROPERTIES OUTPUT_NAME Rayman2 MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
target_include_directories(Rayman2 PRIVATE ${R2_DIR}/include $<$<COMPILE_LANGUAGE:RC>:${CMAKE_BINARY_DIR}>)
# NO_STRICT: VC5-era windows.h typed handles as void * (STRICT was opt-in); the sources declare
# Win32 APIs that way.
# RAYMAN2_WINDOW_ICON: the window class loads icon group 1 of Rayman2.rc (DEVIATIONS 12).
# _DLL_COMPILATION_MODE (R2_FORK_DEFINES unless R2_SND_LINKED): as tu.json, the retail exe's sound
# binding layer (SND loads its drivers as DLLs).
target_compile_definitions(Rayman2 PRIVATE inline=__inline RAYMAN2_STANDALONE NO_STRICT RAYMAN2_WINDOW_ICON ${R2_FORK_DEFINES})
# WIN32: the Windows target macro (docs/PLAN.md "Platform macros"). MSVC got it only from CMake's
# default flags, clang-cl did not.
# _CRT_SECURE_NO_WARNINGS: the sources use the standard C string and file functions (strcpy,
# sprintf, fopen, ...) on every target; the Microsoft C runtime flags them as deprecated in favour
# of its own *_s variants, which the other targets do not have.
if(WIN32)
  target_compile_definitions(Rayman2 PRIVATE WIN32 _CRT_SECURE_NO_WARNINGS)
  if(TARGET GliDX6vf)
    target_compile_definitions(GliDX6vf PRIVATE WIN32 _CRT_SECURE_NO_WARNINGS)
  endif()
  foreach(drv WAVMWBVR APMmxBVR)
    if(TARGET ${drv})
      target_compile_definitions(${drv} PRIVATE WIN32)
    endif()
  endforeach()
endif()
# Debug (--config Debug): CMake's /Od /RTC1 instead, to run the regression with run-time checks
# (docs/BUGS.md). /O2 would conflict with /RTC1.
target_compile_options(Rayman2 PRIVATE $<$<NOT:$<CONFIG:Debug>>:/O2 /Gy>)
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  # Same policy as MSVC, which only warns on these (C4133, C4013, C4047): Phase 4 cleans them.
  target_compile_options(Rayman2 PRIVATE
    -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration -Wno-error=int-conversion)
  # DINPUT.LIB (VC6) has no SEH tables: link.exe then drops SAFESEH silently, lld-link stops.
  target_link_options(Rayman2 PRIVATE /SAFESEH:NO)
endif()
# /MANIFEST:NO: the linker's default manifest (UAC only) would clash with the one in Rayman2.rc,
# which makes the exe DPI aware.
target_link_options(Rayman2 PRIVATE /MAP /MANIFEST:NO)
if(NOT R2_SND_LINKED)
  # Linked-in drivers call SND directly; only driver DLLs need the exports.
  target_link_options(Rayman2 PRIVATE ${R2_EXPORT_FLAGS})
endif()
target_link_libraries(Rayman2 PRIVATE
  kernel32 user32 gdi32 winmm advapi32 shell32
  # The sources call some CRT functions through their import slots (_imp__fprintf, ...). The UCRT
  # inlines the printf family, so it has no fprintf slot; this library supplies one.
  legacy_stdio_definitions
  ${R2_VC6}/VC98/Lib/DINPUT.LIB dxguid)
if(R2_GLI_DX6)
  # ddraw: DirectDrawCreate and DirectDrawEnumerateExA, the driver's only imports beyond kernel32/user32.
  target_link_libraries(Rayman2 PRIVATE GliDX6vf ddraw)
endif()
if(R2_SND_LINKED)
  target_link_libraries(Rayman2 PRIVATE WAVMWBVR APMmxBVR)
endif()
if(R2_USE_SDL)
  target_link_libraries(Rayman2 PRIVATE SDL3::SDL3)
  if(TARGET WAVMWBVR)
    target_link_libraries(WAVMWBVR PRIVATE SDL3::SDL3)
  endif()
endif()
