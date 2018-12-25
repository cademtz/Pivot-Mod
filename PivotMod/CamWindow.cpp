#include "CamWindow.h"
#include "ModManager.h"

/*
 *	Description:
 *		Window to enable / disable camera tools.
 *
 *		This window appears when you select Piv Mod > Camera.
 */

ATOM CCamWindow::wnd_class = 0;
HWND hCamEnabled;

void CCamWindow::OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hCamEnabled = CreateWindow("button", "Enable camera", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 13, 5, 150, 23, hwnd, 0, gBase.GetInstance(), 0);
	SendMessage(hCamEnabled, WM_SETFONT, (WPARAM)gBase.GetFont(), MAKELPARAM(true, 0));
	SendMessage(hCamEnabled, BM_SETCHECK, cam_enabled, 0);
}

#include "Pivot.h"
#include "Camera.h"
LRESULT CCamWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch (msg)
	{
	case WM_CREATE:
		Instance().OnCreate(hwnd, wparam, lparam);
		break;
	case WM_DESTROY:
		CCamWindow::Instance().Destroy();
		DestroyWindow(hCamEnabled);
	case WM_COMMAND:
		if ((HWND)lparam == hCamEnabled)
		{
			Instance().cam_enabled = SendMessage(hCamEnabled, BM_GETCHECK, 0, 0) == BST_CHECKED;
			gCam.UpdateCamera(Instance().CamEnabled());
			Pivot::pMainForm->DrawFigures();
		}
		break;
	default:
		result = DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return result;
}