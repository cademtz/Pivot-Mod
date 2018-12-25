#include "Effects.h"
#include "ModManager.h"

/*
 *	Description:
 *		Example of a custom window with some basic controls.
 *
 *		This window appears when you select Piv Mod > Effects.
 *		
 *		Here, you get to add the menu controls, control input, etc.
 */

ATOM CTestWindow::wnd_class = 0;

HWND handles[3];

void CTestWindow::OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	handles[0] = CreateWindow("button", "Test effects", BS_GROUPBOX | WS_VISIBLE | WS_CHILD, 13, 5, 118, 100, hwnd, 0, gBase.GetInstance(), 0);
	handles[1] = CreateWindow("button", "Outlines", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 20, 100, 23, hwnd, 0, gBase.GetInstance(), 0);
	handles[2] = CreateWindow("button", "Feelin' shaky", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 60, 100, 23, hwnd, 0, gBase.GetInstance(), 0);

	for (HWND handle : handles)
		SendMessage(handle, WM_SETFONT, (WPARAM)gBase.GetFont(), MAKELPARAM(true, 0));

	SendMessage(handles[1], BM_SETCHECK, gFx.outline, 0);
	SendMessage(handles[2], BM_SETCHECK, gFx.shaky, 0);
}

// This is where you handle info and user input
#include "Pivot.h"
LRESULT CALLBACK CTestWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	switch (msg)
	{
	case WM_CREATE: // Run OnCreate to add our controls and stuff
		CTestWindow::Instance().OnCreate(hwnd, wparam, lparam);
		break;
	case WM_DESTROY: // Destroy the window
		CTestWindow::Instance().Destroy();
		for (HWND handle : handles)
			DestroyWindow(handle);
		break;
	case WM_COMMAND:
		if ((HWND)lparam == handles[1])
		{
			gFx.outline = SendMessage(handles[1], BM_GETCHECK, 0, 0) == BST_CHECKED;
			Pivot::pMainForm->DrawFigures();
		}
		if ((HWND)lparam == handles[2])
			gFx.shaky = SendMessage(handles[2], BM_GETCHECK, 0, 0) == BST_CHECKED;
		break;
	default: // Just let Windows do everything else on default for us
		result = DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return result;
}