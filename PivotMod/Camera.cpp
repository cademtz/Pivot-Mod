#include "Camera.h"
#include "Pivot.h"
#include "ModManager.h"

/*
 *	Description:
 *		Main functionality of the camera tool.
 *
 *		This is how the camera is displayed and interacted with
 */

CCamera gCam;

HBRUSH CCamera::hbrBorder = nullptr;
HPEN CCamera::hpLines = nullptr;

CCamera::CCamera()
{
	// Create the Border brush
	if (!hbrBorder)
	{
		LOGBRUSH br_info;
		br_info.lbColor = RGB(0, 128, 255);
		br_info.lbStyle = BS_SOLID;
		hbrBorder = CreateBrushIndirect(&br_info);
	}

	// Create the dashed pen
	if (!hpLines)
	{
		LOGPEN pen_info;
		pen_info.lopnColor = RGB(0, 0, 0);
		pen_info.lopnStyle = PS_DOT;
		pen_info.lopnWidth.x = 1;
		hpLines = CreatePenIndirect(&pen_info);
	}
}

const POINT& CCamera::AbsolutePos()
{
	static POINT absolute;
	auto corner = Pivot::pMainForm->GetCanvasCorner();
	absolute = { cam_pos.x + corner.x, cam_pos.y + corner.y };
	return absolute;
}

bool CCamera::OnLineTo(HDC hdc, int x, int y)
{
	if (bEnabled) // Disable while the camera is shown
		return false;

	x -= cam_pos.x, y -= cam_pos.y;

	POINT prev;
	MoveToEx(hdc, 0, 0, &prev);
	MoveToEx(hdc, prev.x - cam_pos.x, prev.y - cam_pos.y, nullptr);
	gBase.LineTo(hdc, x, y);

	return true;
}

bool CCamera::OnEllipse(HDC hdc, int top, int left, int right, int bottom)
{
	if (bEnabled) // Disable while the camera is shown
		return false;

	gBase.Ellipse(hdc, top - cam_pos.x, left - cam_pos.y, right - cam_pos.x, bottom - cam_pos.y);
	return true;
}

void CCamera::OnDrawOverlay(HDC hdc)
{
	if (!CCamWindow::Instance().CamEnabled())
		return;

	// Make sure our camera is the size of the animation and is relative to the canvas
	auto camSize = Pivot::pMainForm->GetAnimationDimensions();
	auto absPos = AbsolutePos();
	RECT rect = { absPos.x, absPos.y, absPos.x + camSize.x, absPos.y + camSize.y };

	// Set the text, colors, and font for our camera label
	SetTextColor(hdc, bHovered ? RGB(0, 128, 255) : RGB(0, 0, 0));
	auto prevCol = SetBkColor(hdc, RGB(200, 200, 200));
	auto prevFont = SelectObject(hdc, gBase.GetFont());
	// Draw the text, top-left aligned
	DrawText(hdc, "Camera", -1, &rect, DT_LEFT | DT_TOP);

	// Draw the camera's frame
	auto prevBrush = SelectObject(hdc, hbrBorder);
	FrameRect(hdc, &rect, hbrBorder);

	POINT prevPos; // Get original line pos and color
	MoveToEx(hdc, 0, 0, &prevPos);

	// Draw the camera's guides
	auto prevPen = SelectObject(hdc, hpLines);

	MoveToEx(hdc, rect.left + (camSize.x / 3), rect.top, 0);
	gBase.LineTo(hdc, rect.left + (camSize.x / 3), rect.top + camSize.y);

	MoveToEx(hdc, rect.left + (camSize.x / 3) * 2, rect.top, 0);
	gBase.LineTo(hdc, rect.left + (camSize.x / 3) * 2, rect.top + camSize.y);

	MoveToEx(hdc, rect.left, rect.top + (camSize.y / 3), 0);
	gBase.LineTo(hdc, rect.left + camSize.x, rect.top + (camSize.y / 3));

	MoveToEx(hdc, rect.left, rect.top + (camSize.y / 3) * 2, 0);
	gBase.LineTo(hdc, rect.left + camSize.x, rect.top + (camSize.y / 3) * 2);

	// Reset all of the program's old values, so that Pivot doesn't draw with the wrong tools
	SelectObject(hdc, prevBrush);
	SelectObject(hdc, prevFont);
	SelectObject(hdc, prevPen);
	SetBkColor(hdc, prevCol);
	MoveToEx(hdc, prevPos.x, prevPos.y, nullptr);
}

void CCamera::OnMouseMove()
{
	if (!bEnabled)
		return;

	if (bDrag)
	{
		POINT m = Pivot::pMainForm->GetMouse(), pm = Pivot::pMainForm->GetPmouse();
		cam_pos.x += m.x - pm.x, cam_pos.y += m.y - pm.y;
		Pivot::pMainForm->DrawFigures(); // Redraw our camera

		return; // We don't need to know if we're hovered while dragging
	}

	// Make sure our camera is the size of the animation
	auto camSize = Pivot::pMainForm->GetAnimationDimensions();
	auto absPos = AbsolutePos();
	if (!bHovered && gMod.IsMouseOver(absPos.x, absPos.y, camSize.x, camSize.y))
	{
		bHovered = true;
		Pivot::pMainForm->DrawFigures();
	}
	else if (bHovered && !gMod.IsMouseOver(absPos.x, absPos.y, camSize.x, camSize.y))
	{
		bHovered = false;
		Pivot::pMainForm->DrawFigures();
	}
}

void CCamera::OnMouseClick()
{
	// if we aren't hovering and clicking the camera, then don't drag it
	if (!bEnabled || !bHovered || !Pivot::pMainForm->IsLeftClicked())
		return;

	bDrag = true;
	Pivot::pMainForm->DrawFigures();
}

void CCamera::OnMouseRelease()
{
	// If the left mouse is still held, then continue dragging
	if (!bEnabled || Pivot::pMainForm->IsLeftClicked())
		return;

	bDrag = false;
}