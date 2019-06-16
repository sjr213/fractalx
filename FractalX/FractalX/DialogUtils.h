#pragma once

namespace GdiPlus
{
	class Graphics;
	class Rect;
}

namespace DxColor
{
	struct ColorArgb;
}

namespace DlgUtils
{
	void DrawColorSquare(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect, DxColor::ColorArgb color);

	void DrawColorSquare(Gdiplus::Graphics& graphics, const CRect& rect, DxColor::ColorArgb color);

	bool PointInRect(const Gdiplus::Rect& rect, const CPoint& pt);

	void ChooseColor(DxColor::ColorArgb& color, bool enableAlpha, CDialogEx& dlg);

	CString GetFileName(CString &str);
}