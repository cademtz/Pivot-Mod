#include "FrameManager.h"
#include "Pivot.h"

CFrameManager gFrames;
std::vector<CFrame> CFrameManager::frames;

void CFrameManager::OnFramesChanged()
{
	int FrameCount = Pivot::pMainForm->GetFrameCount() + 1;
	if (FrameCount - frames.size() < 0) // Decreased frame count? Destroy frames outside range
		frames.resize(FrameCount);
	else // Otherwise, increased frame count. Copy the last frame's values
		frames.resize(FrameCount, CFrame(frames[frames.size() - 1]));
}