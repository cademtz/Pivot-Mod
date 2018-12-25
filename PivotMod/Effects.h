#pragma once
#include "EffectsWindow.h"

class CEffects
{
public:
	// - Some settings for us to use
	bool outline = false, wacky = false, shaky = false;

	BOOL WINAPI OnLineTo(HDC hdc, int x, int y);
};

extern CEffects gFx;