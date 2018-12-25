#include "ModManager.h"
#include "Base.h"
#include "Pivot.h"

#include "Effects.h"
#include "Camera.h"

/*
 *	Description:
 *		Manages the mod's user options / controls, has events, etc.
 *
 *		Add your options in 'AddOptions' (Obviously...),
 *		Check if your control is being clicked in 'OnPivotWndproc'
 *		I plan to simplify this further so you can just check.
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

#include <windowsx.h>
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

void CModManager::OnDrawOverlay(HDC hdc)
{
	gCam.OnDrawOverlay(hdc);

	RECT idk{ 5, 5, 420, 420 };
	SetTextColor(hdc, RGB(0, 0, 0));
	auto prevCol = SetBkColor(hdc, RGB(200, 200, 200));
	auto prevFont = SelectObject(hdc, gBase.GetFont());

	char buff[32];
	sprintf_s(buff, sizeof(buff), "%i, \t%i", Pivot::pMainForm->GetMouse().x, Pivot::pMainForm->GetMouse().y);

	// Draw the text, top-left aligned
	DrawText(hdc, buff, -1, &idk, DT_LEFT | DT_TOP);

	SetBkColor(hdc, prevCol);
	SelectObject(hdc, prevFont);
}

void CModManager::OnMouseMove()
{
	gCam.OnMouseMove();
}

void CModManager::OnMouseClick()
{
	gCam.OnMouseClick();
}

void CModManager::OnMouseRelease()
{
	gCam.OnMouseRelease();
}

BOOL WINAPI CModManager::OnLineTo(_In_ HDC hdc, _In_ int x, _In_ int y)
{
	gFx.OnLineTo(hdc, x, y);
	if (!gCam.OnLineTo(hdc, x, y))
		return gBase.LineTo(hdc, x, y);
	return false;
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

bool CModManager::IsMouseOver(int x, int y, int w, int h) const
{
	POINT m = Pivot::pMainForm->GetMouse();
	return m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h;
}

bool CModManager::WasMouseOver(int x, int y, int w, int h) const
{
	POINT m = Pivot::pMainForm->GetPmouse();
	return m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h;
}