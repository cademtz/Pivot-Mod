#pragma once

/*
 *	Description:
 *		Dirty C++ so don't look unless you know ASM!
 *
 *		Basic ASM jump hook that can overwrite bytes JMP and execute them later.
 */

class CJumpHook
{
	// - Indicates if this hook is active
	bool hooked = false;
	// - Points to the original function's address
	void* original = nullptr;
	// - Points to overwritten bytes that got relocated
	PBYTE old_code = nullptr;
	// - Holds the length of the overwritten bytes
	size_t code_len = 0;
public:
	template<class T = DWORD>
	inline T GetOriginal() { return (T)original; }
	template<class T = DWORD>
	inline T GetOldCode() { return (T)old_code; }
	inline size_t GetLength() { return code_len; }

	// - Initializes the hook at 'From' and overwrites 'Length' bytes
	// - The hook will replace with a jump to 'To' and will relocate overwritten bytes
	template <class T = DWORD>
	void Hook(T From, void* To, size_t Length)
	{
		DWORD dwOld;
		VirtualProtect((void*)From, Length, PAGE_EXECUTE_READWRITE, &dwOld);

		old_code = (PBYTE)VirtualAlloc(nullptr, Length + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		memcpy_s(old_code, Length, (void*)From, Length);

		old_code[Length] = 0xE9;
		*(DWORD*)&old_code[Length + 1] = ((DWORD)From + Length) - ((DWORD)old_code + Length) - 5;

		((PBYTE)From)[0] = 0xE9; // Insert rel32
		*(DWORD*)((DWORD)From + 1) = (DWORD)To - (DWORD)From - 5;

		VirtualProtect((void*)From, Length, dwOld, &dwOld);

		original = (void*)From, code_len = Length;
		hooked = true;
	}

	// - Restores old code to the original function
	template <class T = unsigned long>
	void UnHook()
	{
		DWORD dwOld;
		VirtualProtect(original, code_len, PAGE_EXECUTE_READWRITE, &dwOld);
		memcpy_s(original, code_len, old_code, code_len);
		VirtualProtect(original, code_len, dwOld, &dwOld);
		VirtualFree(old_code, 0, MEM_RELEASE);

		original = nullptr, code_len = 0;
		hooked = false;
	}
};