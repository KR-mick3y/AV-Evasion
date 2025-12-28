#include <windows.h>
#include <iostream>
using namespace std;

int main() {
	//prepare startupinfo & process_info structure

	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	CreateProcessA("C:\\Windows\\System32\\mspaint.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	
	cout << "Process is created with process id : " << pi.dwProcessId << endl;
	getchar();
	return 0;
}
