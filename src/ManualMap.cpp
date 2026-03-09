#include "ManualMap.h"
#include <iostream>
#include <fstream>
#include <TlHelp32.h>

// Professional Manual Mapping Implementation
// Bypasses standard LoadLibrary detection

typedef HMODULE(WINAPI* pLoadLibraryA)(LPCSTR);
typedef FARPROC(WINAPI* pGetProcAddress)(HMODULE, LPCSTR);
typedef BOOL(WINAPI* pDllMain)(HMODULE, DWORD, LPVOID);

struct MANUAL_MAPPING_DATA
{
    pLoadLibraryA fnLoadLibraryA;
    pGetProcAddress fnGetProcAddress;
    BYTE* pBase;
    HMODULE hModule;
    DWORD fdwReasonParam;
    LPVOID reservedParam;
    BOOL SEHSupport;
};

#define RELOC_FLAG(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW || (RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

void __stdcall Shellcode(MANUAL_MAPPING_DATA* pData);

namespace ManualMap
{
    bool InjectDll(HANDLE hProc, const char* szDllPath)
    {
        std::cout << "[*] Starting manual map injection..." << std::endl;
        
        // Read DLL file
        std::ifstream file(szDllPath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::cout << "[-] Failed to open DLL file: " << szDllPath << std::endl;
            return false;
        }

        auto fileSize = file.tellg();
        if (fileSize < sizeof(IMAGE_DOS_HEADER))
        {
            std::cout << "[-] Invalid DLL file size" << std::endl;
            return false;
        }

        file.seekg(0, std::ios::beg);
        BYTE* pSrcData = new BYTE[(UINT_PTR)fileSize];
        file.read((char*)pSrcData, fileSize);
        file.close();

        std::cout << "[+] DLL file loaded (" << fileSize << " bytes)" << std::endl;

        // Validate PE headers
        IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pSrcData;
        if (pDosHeader->e_magic != 0x5A4D)
        {
            std::cout << "[-] Invalid DOS header" << std::endl;
            delete[] pSrcData;
            return false;
        }

        IMAGE_NT_HEADERS* pNtHeaders = (IMAGE_NT_HEADERS*)(pSrcData + pDosHeader->e_lfanew);
        if (pNtHeaders->Signature != 0x4550)
        {
            std::cout << "[-] Invalid NT headers" << std::endl;
            delete[] pSrcData;
            return false;
        }

        std::cout << "[+] PE headers validated" << std::endl;

        // Allocate memory in target process
        BYTE* pTargetBase = (BYTE*)VirtualAllocEx(hProc, NULL, pNtHeaders->OptionalHeader.SizeOfImage,
            MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        if (!pTargetBase)
        {
            std::cout << "[-] Failed to allocate memory. Error: " << GetLastError() << std::endl;
            delete[] pSrcData;
            return false;
        }

        std::cout << "[+] Allocated memory at: 0x" << std::hex << (uintptr_t)pTargetBase << std::dec << std::endl;

        // Copy headers
        if (!WriteProcessMemory(hProc, pTargetBase, pSrcData, pNtHeaders->OptionalHeader.SizeOfHeaders, NULL))
        {
            std::cout << "[-] Failed to write headers" << std::endl;
            VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
            delete[] pSrcData;
            return false;
        }

        // Copy sections
        IMAGE_SECTION_HEADER* pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
        for (UINT i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++)
        {
            if (pSectionHeader->SizeOfRawData)
            {
                if (!WriteProcessMemory(hProc, pTargetBase + pSectionHeader->VirtualAddress,
                    pSrcData + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData, NULL))
                {
                    std::cout << "[-] Failed to write section: " << pSectionHeader->Name << std::endl;
                    VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
                    delete[] pSrcData;
                    return false;
                }
            }
        }

        std::cout << "[+] Sections mapped successfully" << std::endl;

        // Prepare mapping data
        MANUAL_MAPPING_DATA data = { 0 };
        data.fnLoadLibraryA = LoadLibraryA;
        data.fnGetProcAddress = GetProcAddress;
        data.pBase = pTargetBase;
        data.hModule = NULL;
        data.fdwReasonParam = DLL_PROCESS_ATTACH;
        data.reservedParam = NULL;
        data.SEHSupport = TRUE;

        // Allocate memory for mapping data
        void* pMappingData = VirtualAllocEx(hProc, NULL, sizeof(MANUAL_MAPPING_DATA),
            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (!pMappingData)
        {
            std::cout << "[-] Failed to allocate mapping data" << std::endl;
            VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
            delete[] pSrcData;
            return false;
        }

        WriteProcessMemory(hProc, pMappingData, &data, sizeof(MANUAL_MAPPING_DATA), NULL);

        // Allocate memory for shellcode
        void* pShellcode = VirtualAllocEx(hProc, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!pShellcode)
        {
            std::cout << "[-] Failed to allocate shellcode" << std::endl;
            VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
            VirtualFreeEx(hProc, pMappingData, 0, MEM_RELEASE);
            delete[] pSrcData;
            return false;
        }

        WriteProcessMemory(hProc, pShellcode, Shellcode, 0x1000, NULL);

        std::cout << "[+] Executing shellcode..." << std::endl;

        // Execute shellcode
        HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)pShellcode, pMappingData, 0, NULL);
        if (!hThread)
        {
            std::cout << "[-] Failed to create remote thread. Error: " << GetLastError() << std::endl;
            VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
            VirtualFreeEx(hProc, pMappingData, 0, MEM_RELEASE);
            VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
            delete[] pSrcData;
            return false;
        }

        DWORD waitResult = WaitForSingleObject(hThread, 10000);
        if (waitResult == WAIT_TIMEOUT)
        {
            std::cout << "[-] Shellcode execution timeout" << std::endl;
            TerminateThread(hThread, 1);
            CloseHandle(hThread);
            VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
            VirtualFreeEx(hProc, pMappingData, 0, MEM_RELEASE);
            VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
            delete[] pSrcData;
            return false;
        }

        DWORD exitCode = 0;
        GetExitCodeThread(hThread, &exitCode);
        CloseHandle(hThread);

        // Cleanup
        VirtualFreeEx(hProc, pMappingData, 0, MEM_RELEASE);
        VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
        delete[] pSrcData;

        if (exitCode != 0)
        {
            std::cout << "[-] Shellcode returned error: " << exitCode << std::endl;
            return false;
        }

        std::cout << "[+] Manual map injection successful!" << std::endl;
        std::cout << "[+] Base address: 0x" << std::hex << (uintptr_t)pTargetBase << std::dec << std::endl;

        // Erase PE headers for anti-dump
        ErasePEHeaders(hProc, pTargetBase);

        return true;
    }

    bool InjectDllEx(HANDLE hProcess, const char* dllPath, const char* entryPoint)
    {
        return InjectDll(hProcess, dllPath);
    }

    bool IsProcessProtected(HANDLE hProcess)
    {
        BOOL isProtected = FALSE;
        CheckRemoteDebuggerPresent(hProcess, &isProtected);
        return isProtected;
    }

    bool ErasePEHeaders(HANDLE hProcess, void* baseAddress)
    {
        BYTE zeroBuffer[0x1000] = { 0 };
        SIZE_T written = 0;
        
        if (WriteProcessMemory(hProcess, baseAddress, zeroBuffer, sizeof(zeroBuffer), &written))
        {
            std::cout << "[+] PE headers erased (anti-dump)" << std::endl;
            return true;
        }
        
        return false;
    }
}

// Shellcode for manual mapping
void __stdcall Shellcode(MANUAL_MAPPING_DATA* pData)
{
    if (!pData)
        return;

    BYTE* pBase = pData->pBase;
    auto* pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pBase)->e_lfanew)->OptionalHeader;

    // Resolve imports
    auto* pImportDescr = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (pImportDescr->Name)
    {
        char* szMod = reinterpret_cast<char*>(pBase + pImportDescr->Name);
        HMODULE hDll = pData->fnLoadLibraryA(szMod);

        ULONG_PTR* pThunkRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->OriginalFirstThunk);
        ULONG_PTR* pFuncRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->FirstThunk);

        if (!pThunkRef)
            pThunkRef = pFuncRef;

        for (; *pThunkRef; ++pThunkRef, ++pFuncRef)
        {
            if (IMAGE_SNAP_BY_ORDINAL(*pThunkRef))
            {
                *pFuncRef = (ULONG_PTR)pData->fnGetProcAddress(hDll, reinterpret_cast<char*>(*pThunkRef & 0xFFFF));
            }
            else
            {
                auto* pImport = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pBase + (*pThunkRef));
                *pFuncRef = (ULONG_PTR)pData->fnGetProcAddress(hDll, pImport->Name);
            }
        }
        ++pImportDescr;
    }

    // Process relocations
    if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
    {
        auto* pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        while (pRelocData->VirtualAddress)
        {
            UINT AmountOfEntries = (pRelocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            WORD* pRelativeInfo = reinterpret_cast<WORD*>(pRelocData + 1);

            for (UINT i = 0; i != AmountOfEntries; ++i, ++pRelativeInfo)
            {
                if (RELOC_FLAG(*pRelativeInfo))
                {
                    UINT_PTR* pPatch = reinterpret_cast<UINT_PTR*>(pBase + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
                    *pPatch += reinterpret_cast<UINT_PTR>(pBase) - pOpt->ImageBase;
                }
            }
            pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(pRelocData) + pRelocData->SizeOfBlock);
        }
    }

    // Call DllMain
    auto DllMain = reinterpret_cast<pDllMain>(pBase + pOpt->AddressOfEntryPoint);
    DllMain((HMODULE)pBase, DLL_PROCESS_ATTACH, NULL);

    pData->hModule = (HMODULE)pBase;
}
