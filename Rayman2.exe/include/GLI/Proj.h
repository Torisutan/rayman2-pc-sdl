#pragma once

/* 0x0042c8a0 */
void __cdecl GLI_xSerialProjection(void *p_stCam,
                                    long  lNbOfVertex,
                                    float *p_stSource,
                                    float *p_stDest);

/* 0x0042c7d0 */
void __cdecl GLI_xSerialProjectionNoTrans(void  *p_stCam,
                                           long   lNbOfVertex,
                                           float *p_stSource,
                                           float *p_stDest);

/* 0x00422940 */
void __cdecl GLI_vGet3DVertexFromScreenPos(void  *p_stVpt,
                                            MTH3D_tdstVector *p_stPos3D,
                                            float  xRatioX,
                                            float  xRatioY,
                                            float  xSize);
