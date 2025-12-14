#include <windows.h>
#include <iostream>

int main() {
	// Load user32.dll (MessageBoxW is in user32, not kernel32!)
	HMODULE hUser32 = LoadLibraryW(L"user32.dll");
	if (!hUser32) {
		std::cout << "Failed to load user32.dll" << std::endl;
		return 1;
	}

	// Dynamically resolve MessageBoxW
	std::string first_char = "Message";
	std::string second_char = "BoxW";
	std::string all_char = first_char + second_char;

	FARPROC pMessageBoxWRaw = GetProcAddress(hUser32, all_char.c_str());
	if (!pMessageBoxWRaw) {
		std::cout << "Failed to get MessageBoxW function address" << std::endl;
		FreeLibrary(hUser32);
		return 1;
	}

	typedef int(WINAPI* pMessageBoxW)(
		HWND    hWnd,
		LPCWSTR lpText,
		LPCWSTR lpCaption,
		UINT    uType
	);

	pMessageBoxW MessageBoxW_fn = (pMessageBoxW)pMessageBoxWRaw;
	MessageBoxW_fn(NULL, L"Hello, World", L"Title", MB_OK);

	FreeLibrary(hUser32);
	return 0;
}
