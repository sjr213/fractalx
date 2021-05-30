
#include "stdafx.h"
#include "DialogUtils.h"

#include "ColorArgb.h"
#include "ColorUtils.h"
#include <gdiplus.h>
#include "gdiplustypes.h"
#include "ColorSelectorDlg.h"

using namespace ColorUtils;
using namespace DxColor;
using namespace Gdiplus;

namespace DlgUtils
{
	void DrawColorSquare(Gdiplus::Graphics& graphics, const CRect& rect, DxColor::ColorArgb color)
	{
		Gdiplus::Rect gRect(rect.left, rect.top, rect.Width(), rect.Height());
		DrawColorSquare(graphics, gRect, color);
	}

	void DrawColorSquare(Graphics& graphics, const Rect& rect, ColorArgb color)
	{
#pragma warning(push)
#pragma warning(disable: 4245)
		HatchBrush backGroundBrush(HatchStyle::HatchStyleWideDownwardDiagonal, Color::Black, Color::White);
#pragma warning(pop)

		graphics.FillRectangle(&backGroundBrush, rect);

		SolidBrush colorBrush(ConvertToGdiColor(color));

		graphics.FillRectangle(&colorBrush, rect);

		// Create a Pen object.
		Pen blackPen(Color(255, 0, 0, 0), 2);

		// Draw the rectangle.
		graphics.DrawRectangle(&blackPen, rect);
	}

	bool PointInRect(const Rect& rect, const CPoint& pt)
	{
		if (pt.x < rect.GetLeft() || pt.x > rect.GetRight())
			return false;

		if (pt.y < rect.GetTop() || pt.y > rect.GetBottom())
			return false;

		return true;
	}

	void ChooseColor(DxColor::ColorArgb& color, bool enableAlpha, CDialogEx& dlg)
	{
		auto colorDlg = CColorSelectorDlg::CreateColorSelectorDlg(color, enableAlpha, &dlg);
		if (colorDlg->DoModal() == IDOK)
		{
			color = colorDlg->GetColor();

			dlg.UpdateData(FALSE);
			dlg.Invalidate(TRUE);
		}
	}

	CString GetFileName(CString &str)
	{
		CString fnameStr;

		if (str.GetLength() > 2 && str[1] == ':')
		{
			wchar_t path[_MAX_PATH];

			wcscpy_s(path, _MAX_PATH, str.GetBuffer(1));

			wchar_t drive[_MAX_DRIVE];
			wchar_t dir[_MAX_DIR];
			wchar_t fname[_MAX_FNAME];
			wchar_t ext[_MAX_EXT];
			_wsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

			fnameStr = fname;

		}
		else
			fnameStr = str;

		return fnameStr;
	}
}