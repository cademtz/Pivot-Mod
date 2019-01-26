#pragma once
#include "CamWindow.h"
#include "FrameManager.h"

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
	// - Handle to camera position data in the frame manager, relative to the animation canvas
	CTypeHandle CamPos();
public:
	CCamera();

	// - Returns current camera position at frame 'Frame'
	// - Leave 'Frame' default to change current frame
	const POINT& GetPos(int Frame = -1);
	// - Sets the current camera position at frame 'Frame'
	// - Leave 'Frame' default to change current frame
	void SetPos(const POINT& NewPos, int Frame = -1);
	// - Returns the camera position relative to the whole window at frame 'Frame'
	// - Leave 'Frame' default to change current frame
	const POINT& AbsolutePos(int Frame = -1);

	bool OnLineTo(HDC hdc, int x, int y);
	bool OnEllipse(HDC hdc, int top, int left, int right, int bottom);
	void OnDrawOverlay(HDC hdc);
	void OnMouseMove();
	void OnMouseClick();
	void OnMouseRelease();

	void inline UpdateCamera(bool Enabled) { bEnabled = Enabled; }
};

extern CCamera gCam;