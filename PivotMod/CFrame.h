#pragma once
#include <vector>

class CTypeHandle
{
	size_t Type = 0;
public:
	CTypeHandle() {}
	CTypeHandle(size_t Index) { Type = Index; }
	inline size_t Get() const { return Type; }
};

typedef struct _frame_data
{
	void* ptr;
	size_t size;
} frame_data_t;

class CFrame
{
	// - Is true when a user hasn't yet animated this frame
	bool bNew = true;
	// - Data held by this frame
	std::vector<frame_data_t> frame_data;
public:
	CFrame() {};
	CFrame(const CFrame& Other, bool IsDefault = true);
	~CFrame();

	// - Returns true if this frame hasn't been used / animated yet
	inline bool IsNew() const { return bNew; };
	// - Returns the data stored for that type
	template <class T = void>
	inline T* GetData(CTypeHandle HType) const { return (T*)frame_data[HType.Get()].ptr; }
	// - Updates the data held by this frame
	template <class T>
	inline void Update(CTypeHandle HType, T Data) { *(T*)frame_data[HType.Get()] = Data; }
	// - Creates a new type in the data array and fills it with a default value
	// - IsDefault specifies the frame as new, thus containing a default value
	// - Return value is an index corresponding to your new type, which is needed to update it
	template <class T>
	inline CTypeHandle AddType(T Data, bool IsDefault = true)
	{
		frame_data.push_back({ new T(Data), sizeof(T) });
		if (bNew)
			bNew = IsDefault;
		return frame_data.size() - 1;
	}
};