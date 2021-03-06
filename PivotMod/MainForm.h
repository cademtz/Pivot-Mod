#pragma once
#include "Base.h"

/*
 * Description:
 *		Functions and variables that are part of Pivot's main form
 */

enum class e_mb : char
{
	null = 0,
	ldown,
	rdown
};

typedef struct _AnimInfo
{
	int frame_count;
	char _pad0[8];
	POINT dimensions;
	int _pad1[8];
	int background_count;
} AnimInfo_t;

class CMainForm
{
	// - Holds the current mouse position
	static POINT mouse;
	// - Holds the previous mouse position
	static POINT pmouse;
	// - Holds the current mouse state
	static e_mb mb;

	const AnimInfo_t& GetAnimInfo();
public:
	// - Calls TMainForm::DrawFigures
	// - When changing figures / properties, use this to update the preview afterwards
	void DrawFigures();
	// - Calls TMainForm::DrawAnimBorderLine
	void DrawAnimBorderLine();

	// - Returns the animation dimensions
	inline const POINT& GetAnimationDimensions() { return GetAnimInfo().dimensions; }
	// - Updates and returns bitmap information of Pivot's canvas
	const BITMAP& GetCanvasBitmap();
	// - Updates and returns the top-left corner of Pivot's canvas
	const POINT& GetCanvasCorner();
	// - Returns the device context of Pivot's canvas
	HDC GetCanvasHDC();
	// - Returns number of frames in the current animation
	inline const int GetFrameCount() { return GetAnimInfo().frame_count; }
	// - Returns the current frame
	inline int GetCurrentFrame() const { return *(int*)(this + 0x4B0); }
	// - Returns true if the animation is playing
	inline bool IsPlaying() const { return *(bool*)(*(DWORD *)(this + 0x5B8) + 0x40); }

	// - Returns the current mouse position
	inline const POINT& GetMouse() const { return mouse; }
	// - Returns the previous mouse position
	inline const POINT& GetPmouse() const { return pmouse; }
	// - Retuns the current mouse state
	inline const e_mb& GetMouseState() const { return mb; }
	// - Returns if the left mouse has been clicked
	inline bool IsLeftClicked() const { return mb == e_mb::ldown; }
	// - Returns if the right mouse has been clicked
	inline bool IsRightClicked() const { return mb == e_mb::rdown; }
	// - Returns if the mouse is being held
	inline bool IsMouseHeld() const { return mb != e_mb::null; }

	// - Updates the current and previous mouse position
	// - !! Don't touch this unless a new mouse position has been received! !!
	inline void UpdateMousePos(int x, int y) { pmouse = mouse; mouse.x = x, mouse.y = y; }
	// -  Updates the current mouse state
	// - !! Don't touch this unless a new mouse state has been received! !!
	inline void UpdateMouseDown(BYTE state)
	{
		if (mb != e_mb::null)
			return;

		if (state & 0xB)
			mb = e_mb::ldown;
		else if (state & 0x10)
			mb = e_mb::rdown;
	}
	// -  Updates the current mouse state
	// - !! Don't touch this unless a new mouse state has been received! !!
	inline void UpdateMouseUp(BYTE state)
	{
		if (mb == e_mb::null)
			return;

		if (!state && mb == e_mb::ldown)
			mb = e_mb::null;
		else if (state && mb == e_mb::rdown)
			mb = e_mb::null;
	}

private:
	// Discourage creation / destruction of any instances
	CMainForm() {}
	~CMainForm() {}
};