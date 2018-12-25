#pragma once
#include "CamWindow.h"

/*
 *	Description:
 *		Main functionality of the camera tool.
 *
 *		This is how the camera is displayed and interacted with
 */

class CCamera
{
	// - Brush used to draw camera border
	static HBRUSH hbrBorder;
	// - Pen used to draw dashed "thirds ruler" on the camera
	static HPEN hpLines;
	// - Indicates if the camera is enabled
	bool bEnabled = false;
	// - Indicates if the mouse is hovered over the camera
	bool bHovered = false;
	// - Indicates if we're dragging the camera
	bool bDrag = false;
public:
	CCamera();

	struct {
		LONG x, y;
	} cam_pos = { 11, 11 };
	bool OnLineTo(HDC hdc, int x, int y);
	void OnDrawOverlay(HDC hdc);
	void OnMouseMove();
	void OnMouseClick();
	void OnMouseRelease();

	void inline UpdateCamera(bool Enabled) { bEnabled = Enabled; }
};

extern CCamera gCam;