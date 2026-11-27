# OpenGL 3.3 renderer driver module (docs/PLAN.md "OpenGL driver"): DLL/GliGL3vf.dll beside the
# exe, chosen by ubi.ini (GLI_DllFile=GliGL3, GLI_Dll=OpenGL3) as the retail game chooses its
# driver DLL. Its own files (src/) are the device layer; the Direct3D 6 driver's
# device-independent files are compiled in from GliDX6vf.dll/ (their device calls take the
# USE_GLI_GL branch of GliDX6vf_State.h). SDL3 must be a shared library: the exe and the
# driver use the same SDL (its window, its GL context).

set(R2_GL3_DX6 ${CMAKE_SOURCE_DIR}/GliDX6vf.dll/src)
set(R2_GL3_SOURCES
  src/driver/ogl_drv.c
  src/device/ogl_dev.c
  src/render/ogl_draw.c
  src/texture/ogl_tex.c
  # Direct3D 6 driver files without device calls of their own
  ${R2_GL3_DX6}/core/GliDX6vf_State.c
  ${R2_GL3_DX6}/core/GLI_DiagLog.c
  ${R2_GL3_DX6}/clip/GLI_DRV_vSendSingleLineToClip.c
  ${R2_GL3_DX6}/clip/GLI_DRV_vSendSpriteToClip.c
  ${R2_GL3_DX6}/clip/GLI_DRV_vSendSpriteToClipWithColors.c
  ${R2_GL3_DX6}/clip/GLI_DRV_vSendSpriteToClipWithUV.c
  ${R2_GL3_DX6}/clip/GLI_DRV_xClearViewingList.c
  ${R2_GL3_DX6}/clip/GLI_DRV_xSendListToViewport.c
  ${R2_GL3_DX6}/clip/GLI_SendSpriteTrianglesViaClip.c
  ${R2_GL3_DX6}/clip/clip_pipeline_labels.c
  ${R2_GL3_DX6}/clip/clip_send_triangles.c
  ${R2_GL3_DX6}/clip/clip_staging.c
  ${R2_GL3_DX6}/device/GLI_DRV_bPrepareForGliWindowed.c
  ${R2_GL3_DX6}/device/GLI_DRV_bWindowedModeIsOptimized.c
  ${R2_GL3_DX6}/device/GLI_DRV_vClearZBufferRegion.c
  ${R2_GL3_DX6}/device/GLI_DRV_vNonOptimizedWindowedMode.c
  ${R2_GL3_DX6}/device/GLI_DRV_vOptimizedWindowedMode.c
  ${R2_GL3_DX6}/driver/GLI_DRV_hChangeMode.c
  ${R2_GL3_DX6}/driver/GLI_DRV_lGetSizeOfTexture.c
  ${R2_GL3_DX6}/driver/GLI_DRV_lSetCommonData.c
  ${R2_GL3_DX6}/driver/GLI_DRV_lSetCommonFct.c
  ${R2_GL3_DX6}/driver/GLI_DRV_vActivateGli.c
  ${R2_GL3_DX6}/driver/GLI_DRV_vCloseDriver.c
  ${R2_GL3_DX6}/driver/GLI_DRV_vGetStats.c
  ${R2_GL3_DX6}/render/GLI_DRV_vAddBlackPolygon.c
  ${R2_GL3_DX6}/render/GLI_DRV_vComputeFogEffect.c
  ${R2_GL3_DX6}/render/GLI_DRV_vNoBlackPolygon.c
  ${R2_GL3_DX6}/render/GLI_DRV_vSetClipWindow.c
  ${R2_GL3_DX6}/render/GLI_DRV_vSetZClip.c
  ${R2_GL3_DX6}/render/GLI_DisableZWriteIfEnabled.c
  ${R2_GL3_DX6}/render/GLI_DrawBlackPolygons.c
  ${R2_GL3_DX6}/render/GLI_EnsureZFuncAlways.c
  ${R2_GL3_DX6}/render/GLI_EnsureZFuncGreaterequal.c
  ${R2_GL3_DX6}/render/GLI_EnsureZWriteEnabled.c
  ${R2_GL3_DX6}/render/GLI_SetBeginSceneOpaqueTextureState.c
  ${R2_GL3_DX6}/render/GLI_UpdateFogParameters.c
  ${R2_GL3_DX6}/texture/GLI_DRV_vDoOpaqueTextureSelection.c
  ${R2_GL3_DX6}/texture/GLI_lBitRunLength.c
  ${R2_GL3_DX6}/texture/GLI_lFindDefinedPixelFormat.c
  ${R2_GL3_DX6}/texture/GLI_lFindFirstSetBit.c
  ${R2_GL3_DX6}/texture/GLI_lGetOrAddDefinedPixelFormat.c
  ${R2_GL3_DX6}/texture/GLI_lMakeBitMask.c
  ${R2_GL3_DX6}/texture/GLI_lPixelFormatMasksMatch3.c
  ${R2_GL3_DX6}/texture/GLI_lPixelFormatMasksMatch4.c
  ${R2_GL3_DX6}/texture/GLI_ulGetTextureMemoryCost.c
  ${R2_GL3_DX6}/texture/GLI_vAnalyzeColorMask.c
  ${R2_GL3_DX6}/texture/GLI_vDerivePixelFormatRecord.c
  ${R2_GL3_DX6}/texture/GLI_vGetSelectedTextureFormat.c
  ${R2_GL3_DX6}/texture/GLI_vSelectPixelFormatForTexture.c
  ${R2_GL3_DX6}/texture/GLI_vTextureLoadError.c)

add_library(GliGL3vf SHARED ${R2_GL3_SOURCES})
set_target_properties(GliGL3vf PROPERTIES
  MSVC_RUNTIME_LIBRARY MultiThreadedDLL
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/DLL)
target_include_directories(GliGL3vf PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/GliDX6vf.dll/include
  ${CMAKE_SOURCE_DIR}/Rayman2.exe/include)
target_compile_definitions(GliGL3vf PRIVATE USE_GLI_GL USE_SDL inline=__inline)
if(WIN32)
  target_compile_definitions(GliGL3vf PRIVATE WIN32 _CRT_SECURE_NO_WARNINGS)
endif()
target_compile_options(GliGL3vf PRIVATE $<$<NOT:$<CONFIG:Debug>>:/O2 /Gy>)
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(GliGL3vf PRIVATE
    -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration -Wno-error=int-conversion)
endif()
target_link_libraries(GliGL3vf PRIVATE SDL3::SDL3-shared)
