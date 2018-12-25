#include "Effects.h"
#include "Base.h"

CEffects gFx;

BOOL WINAPI CEffects::OnLineTo(HDC hdc, int x, int y)
{
	// waaackkyyykyy effects!!!!
	if (gFx.outline || gFx.wacky || gFx.shaky)
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
		if (gFx.outline)
		{
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, prev.x - 3, prev.y - 3, nullptr);
			gBase.LineTo(hdc, x, y);
			SelectObject(hdc, hOldPen);
		}
		if (gFx.shaky)
		{
			int random = (rand() % 6) - 3;
			MoveToEx(hdc, prev.x + random, prev.y - random, nullptr);
			gBase.LineTo(hdc, x - random, y - random);
		}
		else
			MoveToEx(hdc, prev.x, prev.y, nullptr);
	}

	return 0;
}
