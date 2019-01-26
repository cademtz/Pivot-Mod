#include "CFrame.h"

CFrame::CFrame(const CFrame & Other, bool IsDefault)
{
	bNew = IsDefault;
	for (auto& otherData : Other.frame_data)
	{
		frame_data_t newData{ new char[otherData.size], otherData.size };
		memcpy_s(newData.ptr, newData.size, otherData.ptr, otherData.size);
		frame_data.push_back(newData);
	}
}

CFrame::~CFrame()
{
	// Destroy all objects we allocated 
	for (size_t i = 0; i < frame_data.size(); i++)
		delete frame_data[i].ptr;
}