#pragma once
#include "CBaseWindow.h"
#include "CMenuItem.h"

/*
 *	Description:
 *		Window to enable / disable camera tools
 *
 *		This window appears when you select Piv Mod > Camera.
 */

class CCamWindow : public CBaseWindow
{
	// - Indicates if we have registered this class already
	static ATOM wnd_class;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// - Indicates if cam is enabled or disabled
	bool cam_enabled = false;

	// - Registers the window's class if not previously done
	// - Privated to allow one instance
	CCamWindow()
	{
		if (wnd_class)
			return;

		InitType("CCamWindow", (HBRUSH)COLOR_BACKGROUND + 1);
		Register(WndProc, wnd_class);
	}
	// - Privated to allow one instance
	~CCamWindow() { }
public:
	// - Return value indicates if camera tools are enabled
	inline bool CamEnabled() { return cam_enabled; }

	// - Creates one, single class for us to use
	static CCamWindow& Instance()
	{
		static CCamWindow cam_window;
		return cam_window;
	}

	virtual void OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam);
};