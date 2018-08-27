
#include "stdafx.h"
#include "PaletteViewDlg.h"
#include "Resource.h"
#include "Messages.h"
#include "DoubleBuffer.h"
#include "ColorUtilities.h"
#include "PinTracker.h"

using namespace DxColor;

class CPaletteViewDlgImp : public CPaletteViewDlg
{
private:
	PinPalette m_palette;
	CWnd* m_parent = nullptr;
	std::function<void(const PinPalette&)> m_newPaletteMethod;
	std::shared_ptr<CDoubleBuffer> m_doubleBuffer;
	CPoint m_topLeft = CPoint(0, 0);
	CRect m_paletteRect;
	bool m_dcNotReady = true;
	bool m_paletteNeedsDrawing = true;
	std::vector<uint32_t> m_colors;
	HICON m_hIcon;
	std::shared_ptr<DxColor::CPinTracker> m_pinTracker;
	int m_activePinIndex = -1;
	CPoint m_pinPt;
	CString m_indexText;

	const int NumberOfColors = 1000;
	const int ColorLineHeight = 10;
	const int BorderWidth = 10;
	const int ControlRegionHeight = 50;

public:
	CPaletteViewDlgImp(const PinPalette& palette, CWnd* pParent)
		: CPaletteViewDlg(IDD_PALETTE_VIEW_DLG, pParent)
		, m_palette(palette)
		, m_parent(pParent)
		, m_doubleBuffer(CDoubleBuffer::CreateBuffer())
	{}

	virtual ~CPaletteViewDlgImp()
	{
		if (m_hIcon)
			DestroyIcon(m_hIcon);
	}

	void SetNewPaletteMethod(std::function<void(const PinPalette&)> newPaletteMethod) override
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

		m_colors = fx::ColorUtilities::ConvertPalette(m_palette, NumberOfColors);

		return TRUE;
	}

	void InitDoubleBuffer(const CRect& clientRect)
	{
		CSize drawSize(NumberOfColors, ColorLineHeight);
		m_doubleBuffer->SetDrawSize(drawSize);

		CSize displaySize(clientRect.Size());
		if (displaySize.cx > 3 * BorderWidth)
		{
			displaySize.cx -= 2 * BorderWidth;
			m_topLeft.x = BorderWidth;
		}

		if (displaySize.cy > 2 * ControlRegionHeight)
		{
			displaySize.cy -= (2 * ControlRegionHeight);
			m_topLeft.y = ControlRegionHeight;
		}

		m_paletteRect = CRect(m_topLeft, displaySize);

		m_doubleBuffer->SetDisplaySize(displaySize);

		m_pinTracker = std::make_shared<CPinTracker>(displaySize, NumberOfColors, 32, m_palette.Pins, m_topLeft);
	}

	void PaletteChanged()
	{
		m_colors = fx::ColorUtilities::ConvertPalette(m_palette, NumberOfColors);

		m_paletteNeedsDrawing = true;

		UpdateData(FALSE);
		InvalidateRect(m_paletteRect, FALSE);
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPaletteViewDlg::DoDataExchange(pDX);

		if (!pDX->m_bSaveAndValidate)
		{
			auto name = m_palette.Name;
			DDX_Text(pDX, IDC_PALETTE_NAME_EDIT, name);

			DDX_Text(pDX, IDC_STATIC_INDEX, m_indexText);
		}
	}

	DECLARE_MESSAGE_MAP()

	afx_msg void OnOk()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		if (m_newPaletteMethod)
			m_newPaletteMethod(m_palette);

		m_parent->PostMessage(cMessage::tm_killPaletteView, 0, 0);
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
			m_doubleBuffer->PrepDCs(dc);
			m_dcNotReady = false;
		}	
	}

	void DrawPaletteColors(CDC& dc)
	{
		for (int x = 0; x < NumberOfColors; ++x)
		{
			auto uColor = m_colors.at(x);
			COLORREF color = fx::ColorUtilities::ToColorRef(uColor);
			CPen aPen;
			aPen.CreatePen(PS_SOLID, 1, color);

			CPen *pOldPen = (CPen*)dc.SelectObject(&aPen);

			dc.MoveTo(x, 0);
			dc.LineTo(x, ColorLineHeight);

			dc.SelectObject(pOldPen);
		}
	}

	void DrawPalatte()
	{
		if (m_paletteNeedsDrawing)
		{		
			CDC* pDC = m_doubleBuffer->GetDrawingDC();

			if(pDC)
				DrawPaletteColors(*pDC);

			m_paletteNeedsDrawing = false;
		}
	}

	void DrawPins()
	{
		auto pDC = m_doubleBuffer->GetDisplayDC();

		if (!m_pinTracker)
			return;

		int nPins = m_pinTracker->GetNumberOfPins();
		for (int i = 0; i < nPins; ++i)
		{
			CPoint pt = m_pinTracker->GetTopLeft(i);

			pDC->DrawIcon(pt.x, pt.y, m_hIcon);
		}
	}

	void OnPaint()
	{
		CPaintDC dc(this); // device context for painting

		PrepBuffer(dc);
		DrawPalatte();		
		DrawPins();

		m_doubleBuffer->Draw(dc, m_topLeft);
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
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

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (m_activePinIndex >= 0)
		{
			ReleaseCapture();

			int x =  point.x - m_pinPt.x;

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

	void OnImport()
	{
	/*
		CString fileName;
		CString fileExt;
		fileExt.Format(_T(".clr%d"), nOrigColors);
		CString fileType;
		fileType.Format(_T("color files (*%s)|*%s|ALL Files |*.*||"), fileExt, fileExt);

		// Open a Save As dialog to get a name
		CFileDialog MyImportDlg(TRUE, fileExt, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, fileType);

		// Display as modal 
		if (MyImportDlg.DoModal() == IDOK)
		{
			fileName = MyImportDlg.GetPathName();

			ImportColorsFromFile(fileName);
		}

		///////////////////////////////////////////
		////////////////////////////////////////////


		shared_ptr<CImageBasic<ush> > pImage = GetImageCopy();
		if (!pImage)
			return false;

		int nOrigColors = pImage->GetNumColors();
		std::vector<RGBQUAD> Quads = pImage->GetColors();

		// Create a CFile and then a CArchive
		CFile colorFile;

		// do something about exceptions
		CFileException FileExcept;
		if (!colorFile.Open(fileName, CFile::modeRead, &FileExcept))
		{
			CString error;
			wchar_t message[100] = { 0 };
			FileExcept.GetErrorMessage(message, 100);
			error.Format(_T("Error opening file: %s: %s"), fileName, message);
			AfxMessageBox(error, MB_ICONWARNING);
			return false;
		}

		CArchive ar(&colorFile, CArchive::load);
		int nColors;
		// Serialize
		ar >> nColors;

		if (nColors != nOrigColors)
		{
			CString mes;
			mes.Format(_T("Color File has an incorrect number of colors %d, instead of %d"), nColors, nOrigColors);
			AfxMessageBox(mes, MB_ICONWARNING);
			ar.Close();
			colorFile.Close();
			return false;
		}

		// Construct the bitmap
		shared_ptr<CDib> pDib(new CDib());
		if (!pDib)
		{
			AfxMessageBox(_T("Memory Error, Please Exit!"), MB_ICONSTOP);
			ar.Close();
			colorFile.Close();
			return false;
		}
		// Save the original colors for undo
		MakeBackUp();

		// we just have to read thru the bitmap but we don't need it so delete it
		pDib->Serialize(ar);

		// Read colors
		for (int i = 0; i < nColors; i++)
			ar >> Quads[i].rgbRed >> Quads[i].rgbGreen >> Quads[i].rgbBlue;

		// read number of pins - 
		int nPins;
		ar >> nPins;

		// read pins
		ClearList();

		// We read in using a CObList to support old versions
		CTypedPtrList<CObList, CPinn*> PinList;
		PinList.Serialize(ar);

		int i = 0;
		POSITION POS = PinList.GetHeadPosition();
		while (POS && i < nPins)
		{
			CPinn *pPin = PinList.GetNext(POS);
			shared_ptr<CPinn> pNewPin(pPin);
			m_Pins.push_back(pNewPin);
			i++;
		}

		// close up
		ar.Close();
		colorFile.Close();

		pImage->SetColors(Quads);
		SetImage(pImage);

		SetModifiedFlag();
		PinsChanged();
		// Change from 0 to HintZoomContrastMin to stop scrolling 10/31/06
		UpdateAllViews(NULL, HintZoomContrastMin, 0);
		return true;
		*/
	}

	void OnExport()
	{
		CString fileName;
		CString fileExt(_T("pins"));
		CString fileType;
		fileType.Format(_T("color pin files (*%s)|*%s|ALL Files |*.*||"), fileExt, fileExt);

		// Open a Save As dialog to get a name
		CFileDialog exportDlg(FALSE, fileExt, NULL, OFN_PATHMUSTEXIST, fileType);

		// Display as modal 
		if (exportDlg.DoModal() != IDOK)
			return;

		fileName = exportDlg.GetPathName();

		// Create a CFile and then a CArchive
		CFile colorFile;

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

		DxColor::ExportPins(ar, m_palette);

		ar.Close();
		colorFile.Close();
	}
};

BEGIN_MESSAGE_MAP(CPaletteViewDlgImp, CPaletteViewDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDOK, &CPaletteViewDlgImp::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CPaletteViewDlgImp::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_IMPORT_BUT, &CPaletteViewDlgImp::OnImport)
	ON_BN_CLICKED(IDC_EXPORT_BUT, &CPaletteViewDlgImp::OnExport)
END_MESSAGE_MAP()

std::shared_ptr<CPaletteViewDlg> CPaletteViewDlg::CreatePaletteViewDlg(const PinPalette& palette, CWnd* pParent)
{
	return std::make_shared <CPaletteViewDlgImp>(palette, pParent);
}