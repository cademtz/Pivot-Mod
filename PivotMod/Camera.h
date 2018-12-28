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

	// - Position of the camera, relative to the animation canvas
	POINT cam_pos = { 0, 0 };
	// - Returns the position of the camera relative to the whole window
	const POINT& AbsolutePos();

	bool OnLineTo(HDC hdc, int x, int y);
	bool OnEllipse(HDC hdc, int top, int left, int right, int bottom);
	void OnDrawOverlay(HDC hdc);
	void OnMouseMove();
	void OnMouseClick();
	void OnMouseRelease();

	void inline UpdateCamera(bool Enabled) { bEnabled = Enabled; }
};

extern CCamera gCam;