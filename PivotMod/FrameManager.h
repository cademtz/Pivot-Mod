#pragma once
#include "CFrame.h"

class CFrameManager
{
	// - Array where we store all our animation's frames
	static std::vector<CFrame> frames;
public:
	// - Returns true if this frame hasn't been used / animated yet
	inline bool IsNew(size_t Frame) const { return frames[Frame].IsNew(); };
	// - Returns a frame's data stored for that type
	template <class T = void>
	inline T* GetData(size_t Frame, CTypeHandle HType) const { return (T*)frames[Frame].GetData(HType.Get()); }
	// - Updates the data held by this frame
	template <class T>
	inline void Update(size_t Frame, CTypeHandle HType, T Data) { frames[Frame].Update(HType, Data); }
	// - Creates a new type in the data array and fills it with a default value
	// - IsDefault specifies the frame as new, thus containing a default value
	// - Return value is an index corresponding to your new type, which is needed to update it
	template <class T>
	CTypeHandle AddType(T Data, bool IsDefault = true)
	{
		for (size_t i = 1; i < frames.size(); i++)
			frames[i].AddType(Data, IsDefault); // Add new type to all frames (Keeps new type index same between all frames)

		if (!frames.size())
			frames.push_back(CFrame());
		return frames[0].AddType(Data, IsDefault); // Add new type to first frame and get the new handle
	}
	// - Only called when Pivot's frame count updates
	// - This will fill our frame manage an equal amount of frames
	void OnFramesChanged();
};

extern CFrameManager gFrames;