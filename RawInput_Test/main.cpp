/********************************************************************************

Project http://rawinput.googlecode.com

********************************************************************************/

#include "..\RawInput\RawInputAPI.h"
#include "..\RawInput\RawInput.h"

#include <iomanip>
#include <memory>

typedef RawInput::Input<RawInput::Unbuffered> InputSys;

std::shared_ptr<InputSys> input;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND SetupWindow(HINSTANCE instance);

wchar_t app_name[] = TEXT("RawInput Test");


//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR /*lpCmdLine*/, int /*nShowCmd*/)
int main()
{
	HINSTANCE hInstance = nullptr;

	HWND hwnd = SetupWindow(hInstance);

	input = std::shared_ptr<InputSys>(new InputSys(nullptr/*, MOUSE_FLAGS, KEYB_FLAGS, HID_FLAGS*/));

	(*input)
		.onMouseEvent([&](const RawInput::RawMouse & mouse) {
			if (mouse.Button(RawInput::RawMouse::BUTTON_2_DOWN)) ::PostMessage(hwnd, WM_CLOSE, 0, 0);

			std::wcout
				<< TEXT("[")
				<< std::dec << std::setw(4) << mouse.GetData().usFlags << TEXT(":")
				<< std::hex << std::setw(8) << mouse.GetData().ulButtons << TEXT(":")
				<< std::dec << std::setw(4) << mouse.GetData().usButtonFlags << TEXT(":")
				<< std::dec << std::setw(4) << static_cast<short>(mouse.GetData().usButtonData) << TEXT(":")
				<< std::dec << std::setw(4) << mouse.GetData().ulRawButtons << TEXT(":")
				<< std::dec << std::setw(4) << mouse.GetData().lLastX << TEXT(":")
				<< std::dec << std::setw(4) << mouse.GetData().lLastY << TEXT(":")
				<< std::dec << std::setw(4) << mouse.GetData().ulExtraInformation
				<< TEXT("]")

				<< std::endl;
		})
		.onKeyboardEvent([&](const RawInput::RawKeyboard & keyboard) {
			if (keyboard.KeyDown('Q') || keyboard.KeyUp(VK_END)) ::PostMessage(hwnd, WM_CLOSE, 0, 0);

			std::wcout
				<< (keyboard.GetData().Flags & RI_KEY_BREAK ? TEXT("B") : TEXT("M"))
				
				<< "["
				<< std::setw(4) << keyboard.GetData().MakeCode << ":"
				<< std::setw(4) << keyboard.GetData().Flags << ":"
				<< std::setw(4) << keyboard.GetData().Reserved << ":"
				<< std::setw(4) << keyboard.GetData().VKey << ":"
				<< std::setw(4) << keyboard.GetData().Message << ":"
				<< std::setw(4) << keyboard.GetData().ExtraInformation
				<< "]\n"

				<< std::endl;
		});

	MSG msg = {0};

	while (msg.message != WM_QUIT) {
		while (!::PeekMessage(&msg, nullptr, 0u, 0u, PM_NOREMOVE)) {
			::WaitMessage();
		}

		while (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	input.reset();

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

HWND SetupWindow(HINSTANCE hInstance)
{
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

	return hwnd;
}
