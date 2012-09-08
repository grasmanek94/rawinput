/********************************************************************************

Project http://code.google.com/p/rawinput/

********************************************************************************/

#include "..\RawInput\RawInput.h"

typedef RawInput::Input<RawInput::Unbuffered> InputSys;

InputSys * input = nullptr; // It would be best to use a smart pointer.

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nShowCmd)
{
	wchar_t app_name[] = TEXT("RawInput Test");

	WNDCLASSEX wndclassex = {
		sizeof(WNDCLASSEX),						// cbSize
		CS_HREDRAW|CS_VREDRAW,					// style
		WndProc,								// lpfnWndProc
		0,										// cbClsExtra
		0,										// cbWndExtra
		hInstance,								// hInstance
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
		hInstance,
		nullptr);

	input = new InputSys(hwnd/*, MOUSE_FLAGS, KEYB_FLAGS, HID_FLAGS*/);

	MSG msg = {0};

	while (msg.message != WM_QUIT) {
		if (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {	//Do work...
			if (input->KeyUp(VK_END)) ::PostMessage(hwnd, WM_CLOSE, 0, 0);

			if (input->KeyDown('Q')) ::PostMessage(hwnd, WM_CLOSE, 0, 0);

			if (input->MouseButton(RawInput::RawMouse::BUTTON_1_DOWN)) ::PostMessage(hwnd, WM_CLOSE, 0, 0);

			input->Clean(); // Clean input last...
		}

		/*
		if (mssg.message == WM_KEYDOWN) {
			WPARAM param = mssg.wParam;
			char c = MapVirtualKey(param,MAPVK_VK_TO_CHAR);
			this->p->Input()->Keyboard()->Listeners()->OnKeyDown(c);
		} else if (mssg.message == WM_KEYUP) {
			WPARAM param = mssg.wParam;
			char c = MapVirtualKey(param,MAPVK_VK_TO_CHAR);
			this->p->Input()->Keyboard()->Listeners()->OnKeyUp(c);
		}
		*/
	}

	delete input; // Remember to delete it

	::UnregisterClass(app_name, hInstance);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		::DestroyWindow(hWnd); return 0;
	case WM_DESTROY:
		::PostQuitMessage(0); return 0;
	case WM_INPUT_DEVICE_CHANGE:
		return input->Change(wParam, lParam); // WIP - Only for Windows Vista or greater.
	case WM_INPUT:
		return input->Update(hWnd, message, wParam, lParam);
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}
