
#include "stdafx.h"
#include "PaletteViewDlg.h"

#include "ContrastDlg.h"
#include "ColorUtils.h"
#include "ColorUtilities.h"
#include "DialogUtils.h"
#include "DoubleBuffer.h"
#include <Gdiplus.h>
#include "Messages.h"
#include "PaletteSelectionDlg.h"
#include "PinTracker.h"
#include "PinEditDlg.h"
#include "Resource.h"
#include "SinglePinEditDlg.h"

using namespace ColorUtils;
using namespace DxColor;
using namespace fx;
using namespace Gdiplus;

class CPaletteViewDlgImp : public CPaletteViewDlg
{
private:
	PinPalette m_palette;
	ColorContrast m_contrast;
	CWnd* m_parent = nullptr;
	std::function<void(const PinPalette&, const ColorContrast&)> m_newPaletteMethod;
	std::shared_ptr<CDoubleBuffer> m_paletteBuffer;
	std::shared_ptr<CDoubleBuffer> m_tickBuffer;
	CPoint m_topLeftPalette = CPoint(0, 0);
	CPoint m_topLeftTicks = CPoint(0, 0);
	CRect m_paletteRect;
	CRect m_tickRect;
	bool m_dcNotReady = true;
	bool m_paletteNeedsDrawing = true;
	bool m_tickBackgroundNeedsDrawing = true;
	std::vector<uint32_t> m_colors;
	HICON m_hIcon;
	std::shared_ptr<DxColor::CPinTracker> m_pinTracker;
	int m_activePinIndex = -1;
	CPoint m_pinPt;
	CString m_indexText;
	std::unique_ptr<CPinEditDlg> m_pinEditDlg;

	const int NumberOfColors = 1000;
	const int ColorLineHeight = 10;
	const int BorderWidth = 10;
	const int ControlRegionHeight = 50;
	const int TickRectHeight = 25;

	bool m_dirty = false;

public:
	CPaletteViewDlgImp(const PinPalette& palette, ColorContrast& contrast, CWnd* pParent)
		: CPaletteViewDlg(IDD_PALETTE_VIEW_DLG, pParent)
		, m_palette(palette)
		, m_contrast(contrast)
		, m_parent(pParent)
		, m_paletteBuffer(CDoubleBuffer::CreateBuffer())
		, m_tickBuffer(CDoubleBuffer::CreateBuffer())
	{}

	virtual ~CPaletteViewDlgImp()
	{
		if (m_hIcon)
			DestroyIcon(m_hIcon);
	}

	void SetNewPaletteMethod(std::function<void(const PinPalette&, const ColorContrast& contrast)> newPaletteMethod) override
	{
		m_newPaletteMethod = newPaletteMethod;
	}

	// Dialog Data
	enum { IDD = IDD_PALETTE_VIEW_DLG };

protected:
	BOOL OnInitDialog() override
	{
		CPaletteViewDlg::OnInitDialog();

		// the Pin - we need to use the instance handle for the dll because that's where the resource (icon) is
		m_hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PIN_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

		CRect clientRect;
		GetClientRect(clientRect);

		InitDoubleBuffer(clientRect);

		m_colors = fx::ColorUtilities::CalculatePaletteColors(m_palette, NumberOfColors, m_contrast);

		EnableUpdate(false);

		return TRUE;
	}

	void InitDoubleBuffer(const CRect& clientRect)
	{
		CSize displaySize(clientRect.Size());
		CPoint topLeft(0, 0);

		if (displaySize.cx > 3 * BorderWidth)
		{
			displaySize.cx -= 2 * BorderWidth;
			topLeft.x = BorderWidth;
		}

		if (displaySize.cy > 2 * ControlRegionHeight)
		{
			displaySize.cy -= (2 * ControlRegionHeight);
			topLeft.y = ControlRegionHeight;
		}

#ifdef DEBUG
		assert(displaySize.cy > ColorLineHeight + TickRectHeight);
#endif
		InitializePaletteBuffer(displaySize, topLeft);
		InitializeTickBuffer(displaySize, topLeft);
	}

	void InitializePaletteBuffer(CSize displaySize, CPoint topLeft)
	{
		CSize drawSize(NumberOfColors, ColorLineHeight);
		m_paletteBuffer->SetDrawSize(drawSize);

		m_topLeftPalette = topLeft;

		CSize paletteSize = CSize(displaySize.cx, displaySize.cy - TickRectHeight);

		m_paletteRect = CRect(m_topLeftPalette, paletteSize);

		m_paletteBuffer->SetDisplaySize(paletteSize);

		m_pinTracker = std::make_shared<CPinTracker>(paletteSize, NumberOfColors, 32, m_palette.Pins, m_topLeftPalette);
	}

	void InitializeTickBuffer(CSize displaySize, CPoint topLeft)
	{
		CSize drawSize(NumberOfColors, TickRectHeight);
		m_tickBuffer->SetDrawSize(drawSize);

		m_topLeftTicks = CPoint(m_topLeftPalette.x, m_topLeftPalette.y + displaySize.cy - TickRectHeight);

		CSize tickSize = CSize(displaySize.cx, TickRectHeight);
		m_tickRect = CRect(m_topLeftTicks, tickSize);

		m_tickBuffer->SetDisplaySize(tickSize);
	}

	void PaletteChanged()
	{
		DxColor::ValidatePalette(m_palette);

		m_colors = fx::ColorUtilities::CalculatePaletteColors(m_palette, NumberOfColors, m_contrast);

		m_paletteNeedsDrawing = true;

		UpdateData(FALSE);
		InvalidateRect(m_paletteRect, FALSE);

		EnableUpdate(true);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPaletteViewDlg::DoDataExchange(pDX);

		auto name = m_palette.Name;

		DDX_Text(pDX, IDC_PALETTE_NAME_EDIT, name);

		if (pDX->m_bSaveAndValidate)
		{
			m_palette.Name = name;
		}
		else
			DDX_Text(pDX, IDC_STATIC_INDEX, m_indexText);
	}

	DECLARE_MESSAGE_MAP()

	afx_msg void OnOk()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		if (m_newPaletteMethod && m_dirty)
			m_newPaletteMethod(m_palette, m_contrast);

		m_parent->PostMessage(cMessage::tm_killPaletteView, 0, 0);
	}

	afx_msg void OnUpdate()
	{
		if (m_newPaletteMethod)
			m_newPaletteMethod(m_palette, m_contrast);

		EnableUpdate(false);
	}

	afx_msg void OnBnClickedCancel()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		m_parent->PostMessage(cMessage::tm_killPaletteView, 0, 0);
	}

	void PrepBuffer(CDC& dc)
	{
		if (m_dcNotReady)
		{
			m_paletteBuffer->PrepDCs(dc);
			m_tickBuffer->PrepDCs(dc);
			m_dcNotReady = false;
		}	
	}

	void DrawPaletteColors(CDC& dc)
	{
		Graphics graphics(dc);

		Gdiplus::Rect gRect(0, 0, NumberOfColors, ColorLineHeight);

		HatchBrush backGroundBrush(Gdiplus::HatchStyle::HatchStyleNarrowHorizontal, Color::Black, Color::White);

		graphics.FillRectangle(&backGroundBrush, gRect);

		int yEnd = ColorLineHeight;

		for (int x = 0; x < NumberOfColors; ++x)
		{
			// std::vector<uint32_t>
			// need ColorArgb 
			auto uColor = m_colors.at(x);
			Pen pen(ConvertToGdiColor(uColor), 1.0);

			graphics.DrawLine(&pen, x, 0, x, yEnd);
		}
	}

	void DrawPalatte()
	{
		if (m_paletteNeedsDrawing)
		{		
			CDC* pDC = m_paletteBuffer->GetDrawingDC();

			if(pDC)
				DrawPaletteColors(*pDC);

			m_paletteNeedsDrawing = false;
		}
	}

	void DrawTickBackground(CDC& dc)
	{
		Graphics graphics(dc);

		Gdiplus::Rect gRect(0, 0, NumberOfColors, TickRectHeight);

		SolidBrush brush(Color::White);

		graphics.FillRectangle(&brush, gRect);
	}

	void DrawTicks(CDC& dc)
	{
		Graphics graphics(dc);

		Pen pen(Color::Black);

		graphics.DrawLine(&pen, 0, 5, 0, TickRectHeight);

		for (int i = 9; i < NumberOfColors; i += 10)
		{
			if(i % 100 == 99)
				graphics.DrawLine(&pen, i, 5, i, TickRectHeight);
			else
				graphics.DrawLine(&pen, i, 15, i, TickRectHeight);
		}
	}

	void DrawTickBackground()
	{
		if (m_tickBackgroundNeedsDrawing)
		{
			CDC* pDC = m_tickBuffer->GetDrawingDC();

			if (pDC)
			{
				DrawTickBackground(*pDC);
				DrawTicks(*pDC);
			}

			m_tickBackgroundNeedsDrawing = false;
		}
	}

	void DrawPins()
	{
		auto pDC = m_paletteBuffer->GetDisplayDC();

		if (!m_pinTracker)
			return;

		int nPins = m_pinTracker->GetNumberOfPins();
		for (int i = 0; i < nPins; ++i)
		{
			CPoint pt = m_pinTracker->GetTopLeft(i);

			pDC->DrawIcon(pt.x, pt.y, m_hIcon);
		}
	}

	void DrawTicksPrep()
	{
		auto pDC = m_tickBuffer->GetDisplayDC();
	}

	void OnPaint()
	{
		CPaintDC dc(this); // device context for painting

		PrepBuffer(dc);
		DrawPalatte();		
		DrawPins();

		DrawTickBackground();
		DrawTicksPrep();

		m_paletteBuffer->Draw(dc, m_topLeftPalette);
		m_tickBuffer->Draw(dc, m_topLeftTicks);
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
		UpdateData(TRUE);

		m_activePinIndex = m_pinTracker->GetIndex(point);

		if (m_activePinIndex >= 0)
		{
			SetCapture();
			m_pinPt = point;
		}

		CPaletteViewDlg::OnLButtonDown(nFlags, point);
	}

	void UpdateIndex(int index)
	{
		if (index < 0)
			m_indexText = _T("");
		else
			m_indexText.Format(_T("Index: %d"), index);

		UpdateData(FALSE);
	}

	bool DeletePin(const CPoint& pt, const CPoint& pinPt)
	{
		int y = pt.y - m_pinPt.y;

		if (y > -30)
			return false;

		if (AfxMessageBox(_T("Delete Pin?"), MB_YESNO) == IDYES)
		{
			m_palette.Pins.erase(std::begin(m_palette.Pins) + m_activePinIndex);

			m_pinTracker->SetPins(m_palette.Pins);

			PaletteChanged();
			return true;
		}

		return false;
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (m_activePinIndex >= 0)
		{
			ReleaseCapture();

			int x =  point.x - m_pinPt.x;
			
			if (DeletePin(point, m_pinPt))
				return;

			// use this to determine new pin position 
			// need to take into account window limits
			m_pinTracker->Move(m_activePinIndex, x);

			m_activePinIndex = -1;

			m_palette.Pins = m_pinTracker->GetPins();

			SpacePins(m_palette);

			PaletteChanged();
		}

		UpdateIndex(-1);

		CPaletteViewDlg::OnLButtonUp(nFlags, point);
	}

	void OnMouseMove(UINT nFlags, CPoint point)
	{
		if (nFlags & MK_LBUTTON && this == GetCapture() && m_activePinIndex >= 0)
		{ 
			int x = point.x - m_pinPt.x;

			m_pinPt.x += x;

			// use this to determine new pin position 
			// need to take into account window limits
			int index = m_pinTracker->Move(m_activePinIndex, x);
			UpdateIndex(index);
		
			InvalidateRect(m_paletteRect, FALSE);
		}

		CPaletteViewDlg::OnMouseMove(nFlags, point);
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		CMenu pinMenu;
		pinMenu.LoadMenu(IDR_PALETTE_VIEW_CONTEXT_MENU);

		m_pinPt = point;

		ClientToScreen(&point);

		pinMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}

	void OnUpdateDeletePin(CCmdUI* cmdUI)
	{
		int nPin = m_pinTracker->GetIndex(m_pinPt);

		cmdUI->Enable(nPin >= 0);
	}

	void OnDeletePin()
	{
		int nPin = m_pinTracker->GetIndex(m_pinPt);
		if (nPin < 0)
			return;
		 
		m_palette.Pins.erase(std::begin(m_palette.Pins) + nPin);

		m_pinTracker->SetPins(m_palette.Pins);

		PaletteChanged();
	}

	void OnKillfocusEdit()
	{
		UpdateData(TRUE);
	}

	void OnImport()
	{
		CString fileName;
		CString fileExt = ColorUtilities::GetPaletteFileExtension();
		CString fileType;
		fileType.Format(_T("color pin files (*%s)|*%s|ALL Files |*.*||"), fileExt, fileExt);

		// Open a Save As dialog to get a name
		CFileDialog MyImportDlg(TRUE, fileExt, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, fileType);
		if (MyImportDlg.DoModal() != IDOK)
			return;
			
		fileName = MyImportDlg.GetPathName();

		auto palette = ColorUtilities::LoadPalette(fileName);
		if (!palette)
			return;

		m_palette = *palette;
		m_pinTracker->SetPins(m_palette.Pins);

		PaletteChanged();
	}

	void OnExport()
	{
		CString fileName = m_palette.Name;
		CString fileExt = ColorUtilities::GetPaletteFileExtension();
		CString fileType;
		fileType.Format(_T("color pin files (*%s)|*%s|ALL Files |*.*||"), fileExt, fileExt);

		// Open a Save As dialog to get a name
		CFileDialog exportDlg(FALSE, fileExt, fileName, OFN_PATHMUSTEXIST, fileType);

		// Display as modal 
		if (exportDlg.DoModal() != IDOK)
			return;

		fileName = exportDlg.GetPathName();

		// Create a CFile and then a CArchive
		CFile colorFile;

		PinPalette paletteCopy = m_palette;
		paletteCopy.Name = DlgUtils::GetFileName(fileName);

		// do something about exceptions
		CFileException FileExcept;
		if (! colorFile.Open(fileName, CFile::modeCreate | CFile::modeWrite, &FileExcept))
		{
			CString error;
			wchar_t message[100] = { 0 };
			FileExcept.GetErrorMessage(message, 100);
			error.Format(_T("Error opening file: %s: %s"), fileName, message);
			AfxMessageBox(error, MB_ICONWARNING);
			return;
		}

		CArchive ar(&colorFile, CArchive::store);

		DxColor::SerializePalette(ar, paletteCopy);

		ar.Close();
		colorFile.Close();
	}

	void OnUpdateEditPins(CCmdUI* cmdUI)
	{
		cmdUI->Enable(TRUE);
	}

	void OnEditPins()
	{
		if (m_pinEditDlg)
			return;

		m_pinEditDlg = std::make_unique<CPinEditDlg>(this);
		m_pinEditDlg->SetPins(m_palette.Pins);

		m_pinEditDlg->Create(IDD_PIN_EDIT_DLG, NULL);
	}

	void OnUpdateEditPin(CCmdUI* cmdUI)
	{
		cmdUI->Enable(TRUE);
	}

	void EditPin(int pinIndex)
	{
		auto pins = m_pinTracker->GetPins();
		auto pin = pins.at(pinIndex);
		auto color = pin.Color1;

		auto pinEditDlg = CSinglePinEditDlg::CreateSinglePinEditDlg(pins, pinIndex, true, this);
		if (pinEditDlg->DoModal() == IDOK)
		{
			m_pinTracker->SetPins(pinEditDlg->GetPins());
			m_palette.Pins = m_pinTracker->GetPins();
			PaletteChanged();
		}
	}

	void OnEditPin()
	{
		int index = m_pinTracker->GetIndex(m_pinPt);

		if (index < 0)
		{
			AfxMessageBox(_T("No pin selected!"));
			return;
		}

		EditPin(index);
	}

	void OnAddPin()
	{
		int newIndex = m_pinTracker->AddPin(m_pinPt);
		if (newIndex < 0)
		{
			AfxMessageBox(_T("No space for new pin!"));
			return;
		}

		m_palette.Pins = m_pinTracker->GetPins();
		PaletteChanged();

		EditPin(newIndex);
	}

	void OnUpdateAddPin(CCmdUI* cmdUI)
	{
		cmdUI->Enable(TRUE);
	}

	void OnSpreadPins()
	{
		if (m_pinTracker->SpreadPins())
		{
			m_palette.Pins = m_pinTracker->GetPins();
			PaletteChanged();
		}
	}

	void OnUpdateSpreadPins(CCmdUI* cmdUI)
	{
		cmdUI->Enable(TRUE);
	}

	LRESULT OnPinsChanged(WPARAM, LPARAM)
	{
		if (!m_pinEditDlg)
			return 0;

		m_palette.Pins = m_pinEditDlg->GetPins();
		m_pinTracker->SetPins(m_palette.Pins);

		PaletteChanged();

		return 0;
	}

	LRESULT OnPinEditDlgClosed(WPARAM wparam, LPARAM)
	{
		// OK
		if (wparam == 1 && m_pinEditDlg)
		{
			m_palette.Pins = m_pinEditDlg->GetPins();
			m_pinTracker->SetPins(m_palette.Pins);

			PaletteChanged();
		}

		if (m_pinEditDlg)
		{
			m_pinEditDlg.reset();
		}

		return 0;
	}

	void OnContrast()
	{
		auto pContrastDlg = CContrastDlg::CreateContrastDlg(m_contrast, this);
		if (!pContrastDlg)
			return;

		if (pContrastDlg->DoModal() == IDOK)
		{
			m_contrast = pContrastDlg->GetContrast();
			PaletteChanged();
		}
	}

	void EnableUpdate(bool enable)
	{
		auto pWnd = GetDlgItem(IDC_UPDATE_BUT);
		if (pWnd)
			pWnd->EnableWindow(enable);

		m_dirty = enable;
	}

	void OnBnClickedPalettes()
	{
		auto pPaletteDlg = CPaletteSelectionDlg::CreatePaletteSelectionDlg(this);
		if (pPaletteDlg->DoModal() == IDOK)
		{
			auto palette = pPaletteDlg->GetSelectedPalette();
			if (!palette)
				return;

			m_palette = *palette;
			m_pinTracker->SetPins(m_palette.Pins);

			PaletteChanged();
		}
	}
};

BEGIN_MESSAGE_MAP(CPaletteViewDlgImp, CPaletteViewDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDOK, &CPaletteViewDlgImp::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CPaletteViewDlgImp::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_IMPORT_BUT, &CPaletteViewDlgImp::OnImport)
	ON_BN_CLICKED(IDC_EXPORT_BUT, &CPaletteViewDlgImp::OnExport)
	ON_BN_CLICKED(IDC_CONTRAST_BUT, &CPaletteViewDlgImp::OnContrast)
	ON_BN_CLICKED(IDC_PALETTES_BUT, &CPaletteViewDlgImp::OnBnClickedPalettes)
	ON_EN_KILLFOCUS(IDC_PALETTE_NAME_EDIT, &CPaletteViewDlgImp::OnKillfocusEdit)
	ON_COMMAND(ID_PALETTE_DELETE_PIN, &CPaletteViewDlgImp::OnDeletePin)
	ON_UPDATE_COMMAND_UI(ID_PALETTE_DELETE_PIN, &CPaletteViewDlgImp::OnUpdateDeletePin)
	ON_COMMAND(ID_PALETTE_EDIT_PINS, &CPaletteViewDlgImp::OnEditPins)
	ON_UPDATE_COMMAND_UI(ID_PALETTE_EDIT_PINS, &CPaletteViewDlgImp::OnUpdateEditPins)
	ON_REGISTERED_MESSAGE(cMessage::tm_pinsChanged, &CPaletteViewDlgImp::OnPinsChanged)
	ON_REGISTERED_MESSAGE(cMessage::tm_pinEditDlgClosed, &CPaletteViewDlgImp::OnPinEditDlgClosed)
	ON_BN_CLICKED(IDC_UPDATE_BUT, &CPaletteViewDlgImp::OnUpdate)
	ON_COMMAND(ID_PALETTE_ADD_PIN, &CPaletteViewDlgImp::OnAddPin)
	ON_UPDATE_COMMAND_UI(ID_PALETTE_ADD_PIN, &CPaletteViewDlgImp::OnUpdateAddPin)
	ON_COMMAND(ID_PALETTE_SPREAD_PINS, &CPaletteViewDlgImp::OnSpreadPins)
	ON_UPDATE_COMMAND_UI(ID_PALETTE_SPREAD_PINS, &CPaletteViewDlgImp::OnUpdateSpreadPins)
	ON_COMMAND(ID_PALETTE_EDIT_PIN, &CPaletteViewDlgImp::OnEditPin)
	ON_UPDATE_COMMAND_UI(ID_PALETTE_EDIT_PIN, &CPaletteViewDlgImp::OnUpdateEditPin)
END_MESSAGE_MAP()

std::shared_ptr<CPaletteViewDlg> CPaletteViewDlg::CreatePaletteViewDlg(const PinPalette& palette, DxColor::ColorContrast& contrast, CWnd* pParent)
{
	return std::make_shared <CPaletteViewDlgImp>(palette, contrast, pParent);
}