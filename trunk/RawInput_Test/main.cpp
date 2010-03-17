/********************************************************************************

Project http://code.google.com/p/rawinput/

********************************************************************************/

#include "..\RawInput\RawInput.h"

#include <iostream>

typedef RawInput::Input<RawInput::UnBuffered> InputUnbuff;

InputUnbuff * input = nullptr; // It would be best to use a smart pointer.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int main(int argc, char * argv[])
{
	WNDCLASSEX wndclassex = {
		sizeof(WNDCLASSEX), // cbSize
		CS_HREDRAW|CS_VREDRAW, // style
		WndProc, // lpfnWndProc
		0, // cbClsExtra
		0, // cbWndExtra
		0, // hInstance
		LoadIcon(0, IDI_APPLICATION), // hIcon
		LoadCursor(0, IDC_ARROW), // hCursor
		(HBRUSH)::GetStockObject(WHITE_BRUSH), // hbrBackground
		0, // lpszMenuName
		TEXT("RawInput Test"), // lpszClassName
		0 // hIconSm
	};

	::RegisterClassEx(&wndclassex);

	HWND hwnd = ::CreateWindowEx(
		WS_EX_APPWINDOW,
		TEXT("RawInput Test"),
		TEXT("RawInput Test"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		::GetDesktopWindow(),
		0,
		0,
		0);

	::ShowWindow(hwnd, true);

	input = new InputUnbuff(hwnd);

	MSG msg = {0};

	do {
		while (!::PeekMessage(&msg, hwnd, 0U, 0U, PM_NOREMOVE)) {
			if (input->KeyUp(VK_END)) ::PostQuitMessage(0);

			if (input->KeyDown(Keyboard::VK_D)) ::PostQuitMessage(0);

			if (input->KeyUp(Keyboard::VK_U)) ::PostQuitMessage(0);

			if (input->MouseButton(Mouse::BUTTON_1_DOWN)) ::PostQuitMessage(0);

			// Clean input...
			input->Clean();
		}

		while (::PeekMessage(&msg, hwnd, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT);

	delete input;

	::UnregisterClass(TEXT("RawInput Test"), 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INPUT_DEVICE_CHANGE: // Works only for Windows Vista or greater.
			return input->Change(wParam, lParam); // Experimental.
		case WM_INPUT:
			return input->Update(hWnd, message, wParam, lParam);
		default:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}