#include "TestWindow.h"
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

HWND handles[4];

void CTestWindow::OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	handles[0] = CreateWindow("button", "Test effects", BS_GROUPBOX | WS_VISIBLE | WS_CHILD, 13, 5, 118, 100, hwnd, 0, gBase.GetInstance(), 0);
	handles[1] = CreateWindow("button", "Outlines", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 20, 100, 23, hwnd, 0, gBase.GetInstance(), 0);
	handles[2] = CreateWindow("button", "Wacky lines", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 40, 100, 23, hwnd, 0, gBase.GetInstance(), 0);
	handles[3] = CreateWindow("button", "Feelin' shaky", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 60, 100, 23, hwnd, 0, gBase.GetInstance(), 0);

	for (HWND handle : handles)
		SendMessage(handle, WM_SETFONT, (WPARAM)gBase.GetFont(), MAKELPARAM(true, 0));

	SendMessage(handles[1], BM_SETCHECK, gMod.outline, 0);
	SendMessage(handles[2], BM_SETCHECK, gMod.wacky, 0);
	SendMessage(handles[3], BM_SETCHECK, gMod.shaky, 0);
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
			gMod.outline = SendMessage(handles[1], BM_GETCHECK, 0, 0) == BST_CHECKED;
			Pivot::MainForm::DrawFigures();
		}
		if ((HWND)lparam == handles[3])
			gMod.shaky = SendMessage(handles[3], BM_GETCHECK, 0, 0) == BST_CHECKED;
		break;
	/*case WM_GETMINMAXINFO: // This is where you set your min / max window size
	{
		LPMINMAXINFO info = (LPMINMAXINFO)lparam;
		info->ptMinTrackSize.x = 300;
		info->ptMinTrackSize.y = 150;

		info->ptMaxTrackSize.x = 300;
		info->ptMaxTrackSize.y = 150;
		break;
	}*/
	default: // Just let Windows do everything else on default for us
		result = DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return result;
}