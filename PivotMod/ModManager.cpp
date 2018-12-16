#include "ModManager.h"
#include "Base.h"

#include "TestWindow.h"
#include "CamWindow.h"

/*
 *	Description:
 *		Manages the mod's user options / controls, has events, etc.
 *
 *		Add your options in 'AddOptions' (Obviously...)
 *		Check if your control is being clicked in 'OnPivotWndproc'
 *		I plan to simplify this further so you can just check
 */

class CModManager gMod;

// - Determines if a control was selected
#define IF_CONTROL(YourControl) if (wparam == YourControl)

// Controls need a unique ID so you can tell them apart
// Just give it whatever name you want and pick a random number!
// 0x just means it's hex, like color codes: #FFFFFF. I just like silly numbers

#define MENU_CAMERA 0x420BEEF
#define MENU_PREFS 0xC00L
#define MENU_ABOUT 123123123

LRESULT WINAPI CModManager::OnPivotWndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_COMMAND: // If we clicked a menu item
	{
		IF_CONTROL(MENU_PREFS)
		{
			if (!CTestWindow::Instance().Get()) // Only run if the window hasn't been opened
			{
				// Get the center pos so our window will start centered on the screen
				RECT center = CBaseWindow::Center(150, 150);

				// Create, show, and update the test window
				CTestWindow::Instance().Create("Effects", 150, 150, center.left, center.top, WS_SYSMENU);
				ShowWindow(CTestWindow::Instance().Get(), SW_SHOW);
				UpdateWindow(CTestWindow::Instance().Get());

				// Resume our window thread
				ResumeThread(gBase.GetMainThread());
			}
		}
		IF_CONTROL(MENU_CAMERA)
		{
			RECT center = CBaseWindow::Center(150, 65);
			CCamWindow::Instance().Create("Camera", 150, 65, center.left, center.top, WS_SYSMENU);
			ShowWindow(CCamWindow::Instance().Get(), SW_SHOW);
			UpdateWindow(CCamWindow::Instance().Get());

			ResumeThread(gBase.GetMainThread());
		}
		IF_CONTROL(MENU_ABOUT)
		{
			// Message box with an OK button and an information icon
			MessageBox(NULL, "This is a basic mod for Pivot!", "About this mod", MB_OK | MB_ICONINFORMATION);
		}
		break;
	}
	}

	// Don't mess with this. This sends all user input to Pivot so it won't freeze
	return CallWindowProc(gBase.GetPivotWndProc(), hwnd, msg, wparam, lparam);
}

BOOL WINAPI CModManager::OnLineTo(HDC hdc, int x, int y)
{
	if (gMod.outline || gMod.wacky || gMod.shaky)
	{
		POINT prev; // Get original line pos and color
		MoveToEx(hdc, 0, 0, &prev);
		// Move line by 3 px and re-draw with lighter color
		static HPEN hPen = nullptr;
		if (!hPen)
		{
			LOGPEN pen_info;
			pen_info.lopnColor = RGB(255, 0, 0);
			pen_info.lopnStyle = PS_SOLID;
			pen_info.lopnWidth.x = 25;
			hPen = CreatePenIndirect(&pen_info);
		}
		if (gMod.outline)
		{
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, prev.x - 3, prev.y - 3, nullptr);
			gBase.LineTo(hdc, x, y);
			SelectObject(hdc, hOldPen);
		}
		if (gMod.shaky)
		{
			int random = (rand() % 6) - 3;
			MoveToEx(hdc, prev.x + random, prev.y - random, nullptr);
			gBase.LineTo(hdc, x - random, y - random);
		}
		else
			MoveToEx(hdc, prev.x, prev.y, nullptr);
	}

	BOOL result = gBase.LineTo(hdc, x, y);

	if (CCamWindow::Instance().CamEnabled())
	{
		RECT rect{ 25, 25, 100, 500 };

		SetTextColor(hdc, RGB(0, 0, 0));
		COLORREF prevCol = SetBkColor(hdc, RGB(200, 200, 200));
		HFONT hOldFont = (HFONT)SelectObject(hdc, gBase.GetFont());
		DrawText(hdc, "Camera", -1, &rect, DT_LEFT | DT_TOP);
		SelectObject(hdc, hOldFont);

		SetBkColor(hdc, RGB(0, 64, 128));

		static HBRUSH hBrush = nullptr;
		if (!hBrush)
		{
			LOGBRUSH br_info;
			br_info.lbColor = RGB(0, 128, 255);
			br_info.lbStyle = BS_SOLID;
			hBrush = CreateBrushIndirect(&br_info);
		}

		rect = RECT{ 25, 45, 25 + 450, 45 + 300 };
		auto hOld = SelectObject(hdc, hBrush);
		FrameRect(hdc, &rect, hBrush);
		SelectObject(hdc, hOld);

		static HPEN hPen = nullptr;
		if (!hPen)
		{
			LOGPEN pen_info;
			pen_info.lopnColor = RGB(255, 0, 0);
			pen_info.lopnStyle = PS_DOT;
			pen_info.lopnWidth.x = 1;
			hPen = CreatePenIndirect(&pen_info);
		}

		POINT prev; // Get original line pos and color
		MoveToEx(hdc, 0, 0, &prev);

		hOld = SelectObject(hdc, hPen);

		int _x = 25, _y = 45, w = 450, h = 300;

		MoveToEx(hdc, _x + (w / 3), _y, 0);
		gBase.LineTo(hdc, _x + (w / 3), _y + h);

		MoveToEx(hdc, _x + (w / 3) * 2, _y, 0);
		gBase.LineTo(hdc, _x + (w / 3) * 2, _y + h);

		MoveToEx(hdc, _x, _y + (h / 3), 0);
		gBase.LineTo(hdc, _x + w, _y + (h / 3));

		MoveToEx(hdc, _x, _y + (h / 3) * 2, 0);
		gBase.LineTo(hdc, _x + w, _y + (h / 3) * 2);

		SelectObject(hdc, hOld);
		MoveToEx(hdc, prev.x, prev.y, nullptr);
	}
	
	return gBase.LineTo(hdc, x, y);
}

void CModManager::AddOptions(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi)
{
	// Make new menu items we can click
	// Assign them a unique number. That number is used to tell which option was clicked
	// You can 
	CMenuItem camera("Camera", MENU_CAMERA);
	CMenuItem effects("Effects", MENU_PREFS);
	CMenuItem about("About", MENU_ABOUT);

	// Add our items to a tab called 'Piv Mod'
	CSubMenu pivmod("Piv Mod");
	pivmod.AddItem(camera);
	pivmod.AddItem(effects);
	pivmod.AddItem(about);

	InsertMenuItem(hmenu, item, fByPosition, pivmod.GetInfo()); // Insert our Piv Mod tab
}

void CModManager::RunWindows()
{
	// Runs all our windows and pauses if they're closed
	bool bRunning = false;
	if (CTestWindow::Instance().Get() && CTestWindow::Instance().Run())
		bRunning = true;

	if (CCamWindow::Instance().Get() && CCamWindow::Instance().Run())
		bRunning = true;

	if (!bRunning)
		SuspendThread(gBase.GetMainThread());
}