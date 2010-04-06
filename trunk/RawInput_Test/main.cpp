/********************************************************************************

Project http://code.google.com/p/rawinput/

********************************************************************************/

#include "..\RawInput\RawInput.h"

typedef RawInput::Input<RawInput::UnBuffered> InputSys;

InputSys * input = nullptr; // It would be best to use a smart pointer.

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int main(int argc, char * argv[])
{
	wchar_t * app_name = TEXT("RawInput Test");

	WNDCLASSEX wndclassex = {
		sizeof(WNDCLASSEX),						// cbSize
		CS_HREDRAW|CS_VREDRAW,					// style
		WndProc,								// lpfnWndProc
		0,										// cbClsExtra
		0,										// cbWndExtra
		nullptr,								// hInstance
		::LoadIcon(nullptr, IDI_APPLICATION),	// hIcon
		::LoadCursor(nullptr, IDC_ARROW),		// hCursor
		(HBRUSH)::GetStockObject(NULL_BRUSH),	// hbrBackground
		nullptr,								// lpszMenuName
		app_name,								// lpszClassName
		::LoadIcon(nullptr, IDI_APPLICATION)	// hIconSm
	};

	::RegisterClassEx(&wndclassex);

	HWND hwnd = ::CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		app_name,
		app_name,
		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		::GetDesktopWindow(),
		nullptr,
		nullptr,
		nullptr);

	input = new InputSys(hwnd/*, MOUSE_FLAGS, KEYB_FLAGS, HID_FLAGS*/);

	MSG msg = {0};

	while (msg.message != WM_QUIT) {
		while (!::PeekMessage(&msg, hwnd, 0u, 0u, PM_NOREMOVE)) {
			//Do work...

			if (input->KeyUp(VK_END)) ::PostQuitMessage(0);

			if (input->KeyDown(Keyboard::VK_D)) ::PostQuitMessage(0);

			if (input->KeyUp(Keyboard::VK_U)) ::PostQuitMessage(0);

			if (input->MouseButton(Mouse::BUTTON_1_DOWN)) ::PostQuitMessage(0);

			// Clean input last...
			input->Clean();
		}

		while (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	delete input;

	::UnregisterClass(app_name, nullptr);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;;
	case WM_INPUT_DEVICE_CHANGE: // Only for Windows Vista or greater. (WIP)
		return input->Change(wParam, lParam);
	case WM_INPUT:
		return input->Update(hWnd, message, wParam, lParam);
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}