#include "ModManager.h"
#include "Base.h"
#include "TestWindow.h"

/*
 *	Description:
 *		Manages the mod's user options / controls
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
				RECT center = CBaseWindow::Center(300, 150);
				// Create, show, and update the test window
				CTestWindow::Instance().Create("Test Window", 300, 150, center.left, center.top, WS_SYSMENU);
				ShowWindow(CTestWindow::Instance().Get(), SW_SHOW);
				UpdateWindow(CTestWindow::Instance().Get());

				// Resume our window thread
				ResumeThread(gBase.GetMainThread());
			}
		}
		IF_CONTROL(MENU_CAMERA)
		{
			// Message box with an OK button. The first argument can always be NULL or 0
			MessageBox(NULL, "Sorry bud, not added yet!", "Too bad", MB_OK);
		}
		IF_CONTROL(MENU_ABOUT)
		{
			// Message box with an OK button and an information icon
			MessageBox(NULL, "This is a basic mod for Pivot!", "About this mod", MB_OK | MB_ICONINFORMATION);
		}
	}
	}

	// Don't mess with this. This sends all user input to Pivot so it won't freeze
	return CallWindowProc(gBase.GetPivotWndProc(), hwnd, msg, wparam, lparam);
}

void CModManager::AddOptions(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi)
{
	// Make new menu items we can click
	// Assign them a unique number. That number is used to tell which option was clicked
	// You can 
	CMenuItem camera("Camera", MENU_CAMERA);
	CMenuItem prefs("Preferences", MENU_PREFS);
	CMenuItem about("About", MENU_ABOUT);

	// Add our items to a tab called 'Piv Mod'
	CSubMenu pivmod("Piv Mod");
	pivmod.AddItem(camera);
	pivmod.AddItem(prefs);
	pivmod.AddItem(about);

	InsertMenuItem(hmenu, item, fByPosition, pivmod.GetInfo()); // Insert our Piv Mod tab
}

void CModManager::RunWindows()
{
	// Runs TestWindow and pauses until necessary to run again
	if (!CTestWindow::Instance().Get() || !CTestWindow::Instance().Run())
		SuspendThread(gBase.GetMainThread());
}