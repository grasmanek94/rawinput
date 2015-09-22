# Introduction #

Right now it's very simple, not much is needed, just:
  * Construct a RawInput::Input<RawInput::Unbuffered> object.
  * Update on user's WndProc function.
  * Subscribe to input events.

The source has a demo project, all the info shown here is replicated there.

Please take notice that this info is likely to change at any time.


---

## Construction ##

Main form of usage is to define a static or extern object:
```
extern/static Input * input; // Preferably inside some namespace/class, respectibly.
```
Then simply:
```
input = new Input(<handle to window>);
```


---

## Update ##

In the user's WndProc function add:
```
case WM_INPUT:
    return input->Update(hWnd, message, wParam, lParam);
```


---

## Events ##

Users can subscribe to mouse/keyboard events:

```
(*input).onMouseEvent([&](const RawInput::RawMouse & mouse) {
	// mouse can be fully inspected here, do something with it.
	})
	.onKeyboardEvent([&](const RawInput::RawKeyboard & keyboard) {
	// keyboard can be fully inspected here, do something with it.
	})
;
```

---
