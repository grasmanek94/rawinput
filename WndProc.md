**http://msdn.microsoft.com/en-us/library/ms633573(VS.85).aspx**

The WndProc implementation should look like this:

```
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INPUT:
		return input->Update(hWnd, message, wParam, lParam);
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}
```

All RawInput\_API data is sent to through WM\_INPUT messages.

The user just returns RawInput::Update and the system deals with it.

RawInput can also deal with WM\_INPUT\_DEVICE\_CHANGE when running on Windows Vista or greater, just add the following to the message switch.

```
	case WM_INPUT_DEVICE_CHANGE:
		return input->Change(wParam, lParam);
```