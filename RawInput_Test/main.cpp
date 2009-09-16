/********************************************************************************

Project http://code.google.com/p/rawinput/

********************************************************************************/

#include "..\RawInput\RawInput.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

RawInput::Input * input = 0;

//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char * argv[])
{
	WNDCLASSEX wndclassex = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW|CS_VREDRAW,
		WndProc,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		TEXT("RawInput"),
		0
	};

	::RegisterClassEx(&wndclassex);

	HWND hwnd = ::CreateWindowEx(
		WS_EX_APPWINDOW,
		TEXT("RawInput"),
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

	input = new RawInput::Input(hwnd);

	MSG msg;

	do {
		if (::PeekMessage(&msg, 0, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		// Do work here...

		input->Clean();
	} while (msg.message != WM_QUIT);

	delete input;

	::UnregisterClass(TEXT("RawInput"), 0);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_INPUT_DEVICE_CHANGE:
			return input->Change(wParam, lParam); // Experimental
		case WM_INPUT:
			return input->Update(hWnd, message, wParam, lParam);
		default:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}