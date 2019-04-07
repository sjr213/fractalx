
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
		HatchBrush backGroundBrush(HatchStyle::HatchStyleWideDownwardDiagonal, Color::Black, Color::White);

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

	void ChooseColor(DxColor::ColorArgb& color, CDialogEx& dlg)
	{
		auto colorDlg = CColorSelectorDlg::CreateColorSelectorDlg(color, &dlg);
		if (colorDlg->DoModal() == IDOK)
		{
			color = colorDlg->GetColor();

			dlg.UpdateData(FALSE);
			dlg.Invalidate(TRUE);
		}
	}
}