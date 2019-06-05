#include "stdafx.h"
#include "PaletteSelectionDlg.h"

#include "ColorPin.h"
#include "ColorUtilities.h"
#include "Resource.h"


using namespace DxColor;
using namespace fx;

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
	std::vector<PaletteData> m_palettes;

public:
	CPaletteSelectionDlgImpl(CWnd* pParent)
		: CPaletteSelectionDlg(IDD_PALETTE_SELECTION_DLG, pParent)
	{}

	virtual ~CPaletteSelectionDlgImpl() {}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CPaletteSelectionDlg::OnInitDialog();

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPaletteSelectionDlg::DoDataExchange(pDX);
	}

	void OnPaint()
	{
//		CPaintDC dc(this);
//		Graphics graphics(dc);
	}

	void OnBrowse()
	{
		CFolderPickerDialog folderdlg;
		if (folderdlg.DoModal() != IDOK)
			return;

		auto path = folderdlg.GetPathName();

		m_palettes = LoadPalettes(path);
	}
};


BEGIN_MESSAGE_MAP(CPaletteSelectionDlgImpl, CPaletteSelectionDlg)
	ON_WM_PAINT()
//	ON_EN_KILLFOCUS(IDC_RED_EDIT, &CSinglePinEditDlgImpl::OnKillFocusRGB)
	ON_BN_CLICKED(IDC_BROWSE_BUT, &CPaletteSelectionDlgImpl::OnBrowse)
//	ON_CBN_SELCHANGE(IDC_COLOR_COMBO, &CSinglePinEditDlgImpl::OnCbnSelchangeColorCombo)
END_MESSAGE_MAP()

std::shared_ptr<CPaletteSelectionDlg> CPaletteSelectionDlg::CreatePaletteSelectionDlg(CWnd* pParent /* = nullptr*/)
{
	return std::make_shared<CPaletteSelectionDlgImpl>(pParent);
}