#ifndef __MMG_SNA_pc_h__
#define __MMG_SNA_pc_h__

typedef HANDLE SNA_pc_tdhFile;

#define SNA_pc_M_hOpenFile(szPath, ulAccess, ulCreation, ulFlags) \
    CreateFileA((szPath), (ulAccess), 0, NULL, (ulCreation), (ulFlags), NULL)
#define SNA_pc_M_bReadFile(hFile, p_vBuffer, ulBytes, p_ulRead) \
    ReadFile((hFile), (p_vBuffer), (ulBytes), (p_ulRead), NULL)
#define SNA_pc_M_ulSeekFile(hFile, lDistance, ulOrigin) \
    SetFilePointer((hFile), (lDistance), NULL, (ulOrigin))
#define SNA_pc_M_bCloseFile(hFile) CloseHandle(hFile)
#define SNA_pc_M_ulGetFileAttributes(szPath) GetFileAttributesA(szPath)
#define SNA_pc_M_bSetFileAttributes(szPath, ulAttributes) SetFileAttributesA((szPath), (ulAttributes))

#endif
