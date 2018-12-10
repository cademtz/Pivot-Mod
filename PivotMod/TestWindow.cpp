#include "TestWindow.h"

/*
 *	Description:
 *		Example of a custom window with some basic controls.
 *
 *		This window appears when you select Piv Mod > Preferences.
 *		
 *		Here, you get to add the menu controls, control input, etc.
 */

ATOM CTestWindow::wnd_class = 0;

void CTestWindow::OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	// Testing my simplified toolbar classes
	CMenuItem neat("Neat", 0);

	CSubMenu cool("Cool");
	CMenuItem cooler("Cooler", 1);
	cool.AddItem(cooler);

	// One toolbar, to hold them all
	HMENU toolbar = CreateMenu();

	// Simply adds a toolbar item to an existing one
	AppendMenu(toolbar, neat.GetInfo()->fType, 0, neat.GetInfo()->dwTypeData);
	AppendMenu(toolbar, MF_POPUP, (UINT_PTR)cool.GetList(), cool.GetInfo()->dwTypeData);

	SetMenu(hwnd, toolbar);

	HWND btn = CreateWindowA("button", "Cool group", BS_GROUPBOX | WS_VISIBLE | WS_CHILD, 10, 10, 170, 50, hwnd, 0, gBase.GetInstance(), 0);
	CreateWindowA("button", "Cool button", WS_VISIBLE | WS_CHILD, 10, 20, 150, 23, btn, 0, gBase.GetInstance(), 0);
}

// This is where you handle info and user input

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
		break;
	case WM_GETMINMAXINFO: // This is where you set your min / max window size
	{
		LPMINMAXINFO info = (LPMINMAXINFO)lparam;
		info->ptMinTrackSize.x = 300;
		info->ptMinTrackSize.y = 150;

		info->ptMaxTrackSize.x = 300;
		info->ptMaxTrackSize.y = 150;
		break;
	}
	default: // Just let Windows do everything else on default for us
		result = DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return result;
}