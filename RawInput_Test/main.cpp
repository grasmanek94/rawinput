/********************************************************************************

Project http://rawinput.googlecode.com

********************************************************************************/

#include <RawInput\RawInputAPI.h>
#include <RawInput\RawInput.h>

#include <iostream>
#include <iomanip>
#include <memory>

#include <tchar.h>

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

HWND SetupWindow( HINSTANCE & );

const wchar_t app_name[] = TEXT("RawInput Test");

typedef RawInput::Input<> InputSys; // Instantiation with default template parameter.

std::shared_ptr<InputSys> input;

int _tmain( int, TCHAR*[] )
{
	HINSTANCE instance;
	HWND hwnd = SetupWindow(instance);

	input = std::make_shared<InputSys>(hwnd);

	(*input)
		.connect(RawInput::RawMouse::Event([&]( const RawInput::RawMouse & mouse )
		{
			const char delim = ',';

			std::wcout
				<< TEXT("[") << std::hex << mouse.GetHandle() << TEXT("]") << std::dec

				<< std::dec << std::setw(4) << mouse.GetData().usFlags << delim

				<< std::hex << std::setw(8) << mouse.GetData().ulButtons << delim

				<< std::dec << std::setw(4) << mouse.GetData().usButtonFlags << delim

				<< std::dec << std::setw(4) << static_cast<short>(mouse.GetData().usButtonData) << delim

				<< std::dec << std::setw(4) << mouse.GetData().ulRawButtons << delim

				<< std::dec << std::setw(4) << mouse.GetData().lLastX << delim
				<< std::dec << std::setw(4) << mouse.GetData().lLastY << delim

				<< std::dec << std::setw(4) << mouse.GetData().ulExtraInformation

				<< (mouse.Button(RawInput::RawMouse::BUTTON_1_DOWN|RawInput::RawMouse::BUTTON_2_DOWN) ? "!" : "")

				<< (mouse.GetWheelDelta() == 0 ? ' ' :
					mouse.GetWheelDelta() <  0 ? '<' : '>')

				<< std::endl;
		}))
		.connect(RawInput::RawKeyboard::Event([&]( const RawInput::RawKeyboard & keyboard )
		{
			const char delim = ',';

			std::wcout
				<< TEXT("[") << std::hex << keyboard.GetHandle() << TEXT("]") << std::dec

				<< (keyboard.GetData().Flags & RI_KEY_BREAK ? TEXT("[B]") /*Break*/ : TEXT("[M]") /*Make*/) << delim
				<< std::hex << std::setw(4) << keyboard.GetData().MakeCode << delim
				
				<< std::dec << std::setw(4) << keyboard.GetData().Flags << delim

				<< std::dec << std::setw(4) << keyboard.GetData().Reserved << delim

				<< std::hex << std::setw(4) << keyboard.GetData().VKey << delim

				<< std::hex << std::setw(4) << keyboard.GetData().Message << delim

				<< std::dec << std::setw(4) << keyboard.GetData().ExtraInformation;

			auto vk = keyboard.GetData().VKey;
			if (vk < 'A' || vk > 'Z')
				std::wcout << '(' << std::hex << vk << ')' << std::endl;
			else
				std::wcout << '(' << char(vk) << ')' << std::endl;

			if (keyboard.KeyDown(VK_SPACE) || keyboard.KeyUp(VK_END)) ::PostMessage(hwnd, WM_CLOSE, 0, 0);
		}))
		.connect(RawInput::RawHID::Event([&]( const RawInput::RawHID & hid )
		{
			const char delim = ',';

			std::wcout
				<< TEXT("[") << std::hex << hid.GetHandle() << TEXT("] ")
				<< std::dec
				<< hid.GetData().dwSizeHid << delim
				<< hid.GetData().dwCount << delim
				<< hid.GetData().bRawData // bRawData is actually an array with sizeof(dwSizeHid*dwCount)

				<< std::endl;
		}));

	bool done = false;
	MSG msg;
	do {
		while (!::PeekMessage(&msg, hwnd, 0u, 0u, PM_NOREMOVE)) {
			::WaitMessage();
		}

		while (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
			if (!::IsDialogMessage(hwnd, &msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);

				if (msg.message == WM_QUIT) done = true;
			}
		}
	} while (!done);

	input.reset(); // Its better to destroy the RawInput object before unregistering the class.

	::UnregisterClass(app_name, instance);
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
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

HWND SetupWindow( HINSTANCE & instance )
{
	instance = ::GetModuleHandle(nullptr);

	WNDCLASSEX wndclassex = {
		sizeof(WNDCLASSEX),						// cbSize
		CS_HREDRAW|CS_VREDRAW,					// style
		WndProc,								// lpfnWndProc
		0,										// cbClsExtra
		0,										// cbWndExtra
		instance,								// hInstance
		::LoadIcon(nullptr, IDI_APPLICATION),	// hIcon
		::LoadCursor(nullptr, IDC_ARROW),		// hCursor
		(HBRUSH)::GetStockObject(NULL_BRUSH),	// hbrBackground
		nullptr,								// lpszMenuName
		app_name,								// lpszClassName
		::LoadIcon(nullptr, IDI_APPLICATION)	// hIconSm
	};

	::RegisterClassEx(&wndclassex);

	return ::CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		app_name,
		app_name,
		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		nullptr,
		nullptr,
		instance,
		nullptr);
}
