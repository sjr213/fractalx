#include "stdafx.h"
#include "PaletteSelectionDlg.h"

#include "ColorContrast.h"
#include "ColorPin.h"
#include "ColorUtils.h"
#include "ColorUtilities.h"
#include "Resource.h"


using namespace DxColor;
using namespace fx;
using namespace Gdiplus;

namespace 
{
	struct PaletteData
	{
		PaletteData(const CString& palettePath, std::shared_ptr<PinPalette> palette)
			: PalettePath(palettePath)
			, Palette(palette)
		{}

		CString PalettePath;
		std::shared_ptr<PinPalette> Palette;
	};

	std::vector<PaletteData> LoadPalettes(const CString& folderPath)
	{
		std::vector<PaletteData> palettes;

		CString location = folderPath;
		location += _T("\\*.");
		location += ColorUtilities::GetPaletteFileExtension();

		CFileFind finder;
		BOOL bWorking = finder.FindFile(location);

		while (bWorking)
		{
			bWorking = finder.FindNextFileW();
			auto palettePath = finder.GetFilePath();

			auto palette = ColorUtilities::LoadPalette(palettePath);
			if(palette) 
				palettes.emplace_back(palettePath, palette);
		}

		return palettes;
	}
}

class CPaletteSelectionDlgImpl : public CPaletteSelectionDlg
{
private:
	const int NumberOfColors = 1000;
	const int BitmapHeight = 20;
	ColorContrast m_contrast;

	std::vector<PaletteData> m_palettes;
	CTreeCtrl m_TreeCtrl;
	CImageList m_ImageList;
	CString m_palettePath;
	int m_selectedPalette = -1;

public:
	CPaletteSelectionDlgImpl(CWnd* pParent)
		: CPaletteSelectionDlg(IDD_PALETTE_SELECTION_DLG, pParent)
	{}

	virtual ~CPaletteSelectionDlgImpl() {}

	std::shared_ptr<PinPalette> GetSelectedPalette() override
	{
		if(m_selectedPalette < 0 || m_selectedPalette >= static_cast<int>(m_palettes.size()))
			return nullptr;

		return m_palettes.at(m_selectedPalette).Palette;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CPaletteSelectionDlg::OnInitDialog();

		EnableOK();

		return TRUE;
	}

	bool IsIndexValid()
	{
		return m_selectedPalette >= 0 && m_selectedPalette < static_cast<int>(m_palettes.size());
	}

	void EnableOK()
	{
		CWnd* pWnd = GetDlgItem(IDOK);
		if (pWnd)
		{
			bool enable = IsIndexValid();
			pWnd->EnableWindow(enable);
		}
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPaletteSelectionDlg::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
		DDX_Text(pDX, IDC_PATH_EDIT, m_palettePath);
	}

	void OnBrowse()
	{
		CFolderPickerDialog folderdlg;
		if (folderdlg.DoModal() != IDOK)
			return;

		m_palettePath = folderdlg.GetPathName();

		m_palettes = LoadPalettes(m_palettePath);

		AddPalettesToList();

		UpdateData(FALSE);
	}

	// replace this with a common function
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

	void AddPalettesToList()
	{
		m_TreeCtrl.DeleteAllItems();
		m_ImageList.DeleteImageList();

		int nPalettes = static_cast<int>(m_palettes.size());
		if (nPalettes > 0)
			m_ImageList.Create(1000, 20, ILC_COLOR24, nPalettes, 1);
		else
			m_ImageList.Create(1000, 20, ILC_COLOR24, 1, 1);

		for (PaletteData& pdata : m_palettes)
		{
			// we can consider adding something to remove a palette for which a hbitmap can't be made but this might 
			// not be possible at this point
			auto bm = GetBitmap(*pdata.Palette);
			assert(bm != nullptr);
			if (bm)
			{
				CBitmap bmap;
				if (bmap.Attach(bm))
					m_ImageList.Add(&bmap, &bmap);
			}
		}

		m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

		// now load tree
		for (int j = 0; j < nPalettes; j++)
		{
			CString fname2 = m_palettes[j].PalettePath;

			// GetFileName() is in common.h it returns just the filename with no path
			m_TreeCtrl.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM,	// nMask
				GetFileName(fname2),			// lpszItem
				j,								// nImage
				j,								// nSelectedImage
				TVIS_EXPANDED,					// nState
				NULL,							// nStateMask
				(LPARAM)j,						// lParam
				TVI_ROOT,						// hParent
				NULL);							// hInsertAfter

		}
	}

	void DrawPaletteColors(Graphics& graphics, const std::vector<uint32_t> colors)
	{
		Gdiplus::Rect gRect(0, 0, NumberOfColors, BitmapHeight);

		HatchBrush backGroundBrush(Gdiplus::HatchStyle::HatchStyleNarrowHorizontal, Color::Black, Color::White);

		graphics.FillRectangle(&backGroundBrush, gRect);

		int yEnd = BitmapHeight;

		for (int x = 0; x < NumberOfColors; ++x)
		{
			// std::vector<uint32_t>
			// need ColorArgb 
			auto uColor = colors.at(x);
			Pen pen(ColorUtils::ConvertToGdiColor(uColor), 1.0);

			graphics.DrawLine(&pen, x, 0, x, yEnd);
		}

	}

	HBITMAP GetBitmap(const PinPalette& palette)
	{
		auto colors = fx::ColorUtilities::CalculatePaletteColors(palette, NumberOfColors, m_contrast);

		HBITMAP dcimage;
		Bitmap b(NumberOfColors, BitmapHeight);

		PAINTSTRUCT ps;
		CDC *pDC = BeginPaint(&ps);

		Graphics *graphics2 = Graphics::FromImage(&b);
		DrawPaletteColors(*graphics2, colors);
		b.GetHBITMAP(Color(255, 255, 0), &dcimage);

		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(*pDC);
		SelectObject(hdcMem, dcimage);
		GetObject(dcimage, sizeof(bm), &bm);

		BitBlt(*pDC, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);

		EndPaint(&ps);

		return dcimage;
	}

	void OnSelChangedTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
	{
		HTREEITEM hSelected = m_TreeCtrl.GetSelectedItem();

		m_selectedPalette = -1;
		if (hSelected != NULL)
		{
			m_selectedPalette = (int)m_TreeCtrl.GetItemData(hSelected);
		}

		EnableOK();

		*pResult = 0;
	}
};


BEGIN_MESSAGE_MAP(CPaletteSelectionDlgImpl, CPaletteSelectionDlg)
//	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CPaletteSelectionDlgImpl::OnNMDblclkTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CPaletteSelectionDlgImpl::OnSelChangedTreeCtrl)
	ON_BN_CLICKED(IDC_BROWSE_BUT, &CPaletteSelectionDlgImpl::OnBrowse)
END_MESSAGE_MAP()

std::shared_ptr<CPaletteSelectionDlg> CPaletteSelectionDlg::CreatePaletteSelectionDlg(CWnd* pParent /* = nullptr*/)
{
	return std::make_shared<CPaletteSelectionDlgImpl>(pParent);
}