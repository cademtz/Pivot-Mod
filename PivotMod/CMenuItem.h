#pragma once
#include "Base.h"

/*
 *	Description:
 *		Simplifies the process of using Windows' ugly API.
 *
 *		I need to improve this, but I might also not :)
 *		Because Pivot is cross-platform and uses a custom style, I'll have to get these controls
 *		to draw in Pivot's style, otherwise they look out of place and ugly.
 */

class CSubMenu;

class CMenuItem
{
	friend class CSubMenu;

	DWORD identifier = 0;
	MENUITEMINFO info{ 0 };
	const CSubMenu* parent = nullptr;
public:
	CMenuItem(const char* szName, DWORD dwID)
	{
		identifier = dwID;

		// Create some properties
		info.cbSize = sizeof(info);
		info.fMask = MIIM_ID | MIIM_TYPE; // A simple item type with an ID
		info.wID = identifier; // Set the ID for checking if item clicked
		info.fType = MFT_STRING; // Only if MIIM_FTYPE. This is a simple labeled item

		// Copy szName into our type's name data
		size_t len = strlen(szName) + 1;
		info.dwTypeData = new char[len];
		strcpy_s(info.dwTypeData, len, szName);
		info.cch = len;
	}

	// - Returns a pointer to the parent
	const CSubMenu* GetParent() const { return parent; }

	// - Returns the identifier of our item
	// - Used for checking if it was clicked
	DWORD ID() const { return identifier; }

	// - Returns the item info
	LPCMENUITEMINFO GetInfo() const { return &info; }
};

class CSubMenu
{
	HMENU menu = nullptr;
	HMENU sub_menu = nullptr;
	MENUITEMINFO info{ 0 };
public:
	CSubMenu(const char* szName)
	{
		menu = CreateMenu();

		// Creating a text item that holds a submenu
		info.cbSize = sizeof(info);
		info.fMask = MIIM_SUBMENU | MIIM_TYPE; // This is a submenu
		info.fType = MFT_STRING; // Only if MIIM_FTYPE. This is a simple labeled item

		// Copy szName into our type's name data
		size_t len = strlen(szName) + 1;
		info.dwTypeData = new char[len];
		strcpy_s(info.dwTypeData, len, szName);
		info.cch = len;

		// Creating the actual submenu to hold items
		info.hSubMenu = sub_menu = CreateMenu();
	}

	// - Adds an item to the menu and changes its parent pointer
	// - Returns true on success
	bool AddItem(CMenuItem& Item)
	{
		Item.parent = this;
		if (!AppendMenu(sub_menu, MF_STRING, Item.ID(), Item.GetInfo()->dwTypeData))
		{
			gBase.Error("Failed to add menu item '%s' to submenu '%s'", Item.GetInfo()->dwTypeData, info.dwTypeData);
			return false;
		}
		return true;
	}

	// - Returns a handle to the item
	HMENU Get() const { return menu; }

	// - Returns a handle to the actual submenu list
	HMENU GetList() const { return sub_menu; }

	// - Returns the item info
	LPCMENUITEMINFO GetInfo() const { return &info; }
};