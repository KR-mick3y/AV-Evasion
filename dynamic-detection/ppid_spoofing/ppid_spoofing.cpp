#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#pragma once
using namespace std;

// ============================================================
// 프로세스 이름으로 PID를 찾는 함수
// ============================================================
DWORD getProcId(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        wcout << L"[-] Failed to take a snapshot" << endl;
        return 0;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                wcout << L"[+] Found process: " << pe.szExeFile << L" (PID: " << pe.th32ProcessID << L")" << endl;
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    wcout << L"[-] Process not found: " << processName << endl;
    return 0;
}

// ============================================================
// PPID 스푸핑하여 프로세스 생성
// ============================================================
BOOL CreateProcessWithSpoofedPPID(
    DWORD parentPid,
    LPCWSTR targetPath,
    DWORD* outPid = NULL,
    DWORD* outError = NULL
) {
    HANDLE hParent = NULL;
    STARTUPINFOEXW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    SIZE_T attrSize = 0;
    BOOL result = FALSE;
    DWORD lastError = 0;

    si.StartupInfo.cb = sizeof(STARTUPINFOEXW);

    // 1. 부모 프로세스 핸들 열기
    hParent = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, parentPid);
    if (!hParent) {
        lastError = GetLastError();
        goto cleanup;
    }

    // 2. 속성 리스트 크기 확인
    InitializeProcThreadAttributeList(NULL, 1, 0, &attrSize);

    // 3. 메모리 할당
    si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        attrSize
    );
    if (!si.lpAttributeList) {
        lastError = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    // 4. 속성 리스트 초기화
    if (!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attrSize)) {
        lastError = GetLastError();
        goto cleanup;
    }

    // 5. 부모 프로세스 속성 추가
    if (!UpdateProcThreadAttribute(
        si.lpAttributeList,
        0,
        PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
        &hParent,
        sizeof(HANDLE),
        NULL,
        NULL
    )) {
        lastError = GetLastError();
        goto cleanup;
    }

    // 6. 프로세스 생성
    {
        SIZE_T pathLen = lstrlenW(targetPath) + 1;
        LPWSTR cmdLine = (LPWSTR)HeapAlloc(
            GetProcessHeap(),
            0,
            pathLen * sizeof(WCHAR)
        );

        if (!cmdLine) {
            lastError = ERROR_OUTOFMEMORY;
            goto cleanup;
        }

        lstrcpyW(cmdLine, targetPath);

        result = CreateProcessW(
            NULL,
            cmdLine,
            NULL,
            NULL,
            FALSE,
            EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si.StartupInfo,
            &pi
        );

        if (!result) {
            lastError = GetLastError();
        }

        HeapFree(GetProcessHeap(), 0, cmdLine);

        if (result && outPid) {
            *outPid = pi.dwProcessId;
        }
    }

cleanup:
    if (si.lpAttributeList) {
        DeleteProcThreadAttributeList(si.lpAttributeList);
        HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
    }
    if (pi.hProcess) CloseHandle(pi.hProcess);
    if (pi.hThread) CloseHandle(pi.hThread);
    if (hParent) CloseHandle(hParent);

    if (outError) {
        *outError = lastError;
    }

    return result;
}
// ============================================================
// main
// ============================================================
int main() {
    wcout << L"=== PPID Spoofing Demo ===" << endl << endl;

    // 1. 스푸핑할 부모 프로세스 찾기
    DWORD parentPid = getProcId(L"notepad.exe");
    if (!parentPid) {
        wcout << L"[-] Please open notepad.exe first" << endl;
        return -1;
    }

    // 2. PPID 스푸핑하여 프로세스 생성
    DWORD createdPid = 0;

    if (CreateProcessWithSpoofedPPID(
        parentPid,
        L"C:\\Windows\\System32\\mspaint.exe", 
        &createdPid
    )) {
        wcout << endl << L"[+] Success!" << endl;
        wcout << L"    Check Process Explorer/Process Hacker to verify PPID" << endl;
        getchar();
    }
    else {
        wcout << endl << L"[-] Failed to create process with spoofed PPID" << endl;
    }

    return 0;
}
