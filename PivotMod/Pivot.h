#pragma once

/*
 *	Description:
 *		Contains organized namespaces with variables and functions from Pivot's code
 *		Useful for making Pivot redraw, load, and save figures as well as other things
 */

// - Namespace with all Pivot-related functions and variables
namespace Pivot
{
	// - Namespace with all functions and variables belonging to Pivot's main form
	namespace MainForm
	{
		// - Calls TMainForm::DrawFigures()
		// - When changing figures / properties, use this to update the preview afterwards
		void DrawFigures();
	}
}