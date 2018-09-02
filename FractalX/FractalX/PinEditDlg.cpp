// PinEditDlg.cpp : implementation file
//
// The parent should allocate and assign m_pPins[] and m_pBackUpPins[] with m_nPins number of pins
// The index of the first Pin shown on the left if m_indexIndex and is zero based
// The each color is shown in a rectangle
// For each Pin there are 6 possible color rectangles
// m_TopLfRect for the CPinn::m_TopLfColor when the pin is slit (different color used for each side of Pin)
// m_TopRtRect for the CPinn::m_TopRtColor when pin is split
// m_BotLfRect for CPinn::m_BotLfColor when pin is split and SPREAD_STRIPE (bands)
// m_BotRtRect for CPinn::m_BotRtColor when pin is split and SPREAD_STRIPE (bands)
// m_TopCtrRect for CPinn::m_TopLfColor when the Pin is NOT split
// m_BotCtrRect for CPinn::m_BotLfColor when the Pin is NOT split and SPREAD_STRIPE
// Color indexes are greater than or equal to 1 and less than or equal to the number of colors
// m_pColors[index-1] <-> m_pPin.SetIndex(index)

#include "stdafx.h"

#include "PinEditDlg.h"
#include "math.h"
#include <algorithm>
#include "ColorUtils.h"

using namespace DxColor;
using namespace ColorUtils;

CPinEditDlg::CPinEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPinEditDlg::IDD, pParent)
	, m_PinNum1(0)
	, m_PinNum2(0)
	, m_PinNum3(0)
	, m_PinIndex1(0)
	, m_PinIndex2(0)
	, m_PinIndex3(0)
	, m_BandA1(DefaultBand)
	, m_BandA2(DefaultBand)
	, m_BandB1(DefaultBand)
	, m_BandB2(DefaultBand)
	, m_k1(0)
	, m_k2(0)
	, m_indexIndex(0)
	, m_IsCopiedColor(FALSE)
	, m_red(_T(""))
	, m_green(_T(""))
	, m_blue(_T(""))
	, m_nPins(0)
	, m_nColors(MaxColorIndex)
	, m_bDirty(FALSE)
{}

CPinEditDlg::~CPinEditDlg()
{}

void CPinEditDlg::SetPins(std::vector<ColorPin>& pins)
{
	m_pins = pins;
	m_backupPins = pins;

	m_nPins = m_nBackUpPins = static_cast<int>(pins.size());

	m_indexIndex = 0;
}

std::vector<ColorPin> CPinEditDlg::GetPins() const
{
	return m_pins;
}

void CPinEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PIN_NUM_EDIT1, m_PinNum1);
	DDX_Text(pDX, IDC_PIN_NUM_EDIT2, m_PinNum2);
	DDX_Text(pDX, IDC_PIN_NUM_EDIT3, m_PinNum3);
	DDX_Text(pDX, IDC_PIN_INDEX_EDIT1, m_PinIndex1);
	DDX_Text(pDX, IDC_PIN_INDEX_EDIT2, m_PinIndex2);
	DDX_Text(pDX, IDC_PIN_INDEX_EDIT3, m_PinIndex3);
	DDX_Text(pDX, IDC_BANDA_EDIT1, m_BandA1);
	DDX_Text(pDX, IDC_BANDA_EDIT2, m_BandA2);
	DDX_Text(pDX, IDC_BANDB_EDIT1, m_BandB1);
	DDX_Text(pDX, IDC_BANDB_EDIT2, m_BandB2);
	DDX_Text(pDX, IDC_CURVE_EDIT1, m_k1);
	DDX_Text(pDX, IDC_CURVE_EDIT2, m_k2);
	DDX_Control(pDX, IDC_RED_EDIT, m_RedEdit);
	DDX_Control(pDX, IDC_GREEN_EDIT, m_GreenEdit);
	DDX_Control(pDX, IDC_BLUE_EDIT, m_BlueEdit);
	DDX_Text(pDX, IDC_RED_EDIT, m_red);
	DDX_Text(pDX, IDC_GREEN_EDIT, m_green);
	DDX_Text(pDX, IDC_BLUE_EDIT, m_blue);
	DDV_MinMaxDouble(pDX, m_BandA1, MinBand, MaxBand);
	DDV_MinMaxDouble(pDX, m_BandA2, MinBand, MaxBand);
	DDV_MinMaxDouble(pDX, m_BandB1, MinBand, MaxBand);
	DDV_MinMaxDouble(pDX, m_BandB2, MinBand, MaxBand);
	// DDV_MinMaxDouble(pDX, m_k1, 0.0001, 1000.0);
	// DDV_MinMaxDouble(pDX, m_k2, 0.001, 1000.0);
}


BEGIN_MESSAGE_MAP(CPinEditDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_NEXT_BUT, &CPinEditDlg::OnBnClickedNextBut)
	ON_BN_CLICKED(IDC_PREVIOUS_BUT, &CPinEditDlg::OnBnClickedPreviousBut)
	ON_BN_CLICKED(IDC_SPREAD_NORM_RAD1, &CPinEditDlg::OnBnClickedSpreadNormRad1)
	ON_BN_CLICKED(IDC_SPREAD_NORM_RAD2, &CPinEditDlg::OnBnClickedSpreadNormRad2)
	ON_BN_CLICKED(IDC_SPREAD_STRIPE_RAD1, &CPinEditDlg::OnBnClickedSpreadStripeRad1)
	ON_BN_CLICKED(IDC_SPREAD_STRIPE_RAD2, &CPinEditDlg::OnBnClickedSpreadStripeRad2)
	ON_BN_CLICKED(IDC_SPREAD_CURVE_RAD1, &CPinEditDlg::OnBnClickedSpreadCurveRad1)
	ON_BN_CLICKED(IDC_SPREAD_CURVE_RAD2, &CPinEditDlg::OnBnClickedSpreadCurveRad2)
	ON_EN_KILLFOCUS(IDC_BANDA_EDIT1, &CPinEditDlg::OnEnKillfocusBandaEdit1)
	ON_EN_KILLFOCUS(IDC_BANDA_EDIT2, &CPinEditDlg::OnEnKillfocusBandaEdit2)
	ON_EN_KILLFOCUS(IDC_BANDB_EDIT1, &CPinEditDlg::OnEnKillfocusBandbEdit1)
	ON_EN_KILLFOCUS(IDC_BANDB_EDIT2, &CPinEditDlg::OnEnKillfocusBandbEdit2)
	ON_EN_KILLFOCUS(IDC_CURVE_EDIT1, &CPinEditDlg::OnEnKillfocusCurveEdit1)
	ON_EN_KILLFOCUS(IDC_CURVE_EDIT2, &CPinEditDlg::OnEnKillfocusCurveEdit2)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_DELETE_BUT1, &CPinEditDlg::OnBnClickedDeleteBut1)
	ON_BN_CLICKED(IDC_DELETE_BUT2, &CPinEditDlg::OnBnClickedDeleteBut2)
	ON_BN_CLICKED(IDC_DELETE_BUT3, &CPinEditDlg::OnBnClickedDeleteBut3)
	ON_BN_CLICKED(IDC_INSERT_BUT1, &CPinEditDlg::OnBnClickedInsertBut1)
	ON_BN_CLICKED(IDC_INSERT_BUT2, &CPinEditDlg::OnBnClickedInsertBut2)
	ON_EN_KILLFOCUS(IDC_PIN_INDEX_EDIT1, &CPinEditDlg::OnEnKillfocusPinIndexEdit1)
	ON_EN_KILLFOCUS(IDC_PIN_INDEX_EDIT2, &CPinEditDlg::OnEnKillfocusPinIndexEdit2)
	ON_EN_KILLFOCUS(IDC_PIN_INDEX_EDIT3, &CPinEditDlg::OnEnKillfocusPinIndexEdit3)
	ON_EN_CHANGE(IDC_PIN_INDEX_EDIT1, &CPinEditDlg::OnEnChangePinIndexEdit1)
	ON_EN_CHANGE(IDC_PIN_INDEX_EDIT2, &CPinEditDlg::OnEnChangePinIndexEdit2)
	ON_EN_CHANGE(IDC_PIN_INDEX_EDIT3, &CPinEditDlg::OnEnChangePinIndexEdit3)
	ON_EN_CHANGE(IDC_BANDA_EDIT1, &CPinEditDlg::OnEnChangeBandaEdit1)
	ON_EN_CHANGE(IDC_BANDB_EDIT1, &CPinEditDlg::OnEnChangeBandbEdit1)
	ON_EN_CHANGE(IDC_BANDA_EDIT2, &CPinEditDlg::OnEnChangeBandaEdit2)
	ON_EN_CHANGE(IDC_BANDB_EDIT2, &CPinEditDlg::OnEnChangeBandbEdit2)
	ON_EN_CHANGE(IDC_CURVE_EDIT1, &CPinEditDlg::OnEnChangeCurveEdit1)
	ON_EN_CHANGE(IDC_CURVE_EDIT2, &CPinEditDlg::OnEnChangeCurveEdit2)
END_MESSAGE_MAP()


// CPinEditDlg message handlers

BOOL CPinEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int left = 124;		// left of left most color boxes
	int top = 88;		// top of the top most color boxes
	int width = 35;		// width AND HEIGHT of each box
	int gap = 5;		// space between boxes for the same pin
	int group2 = 138;	// distance from left of pin 1 to left of pin 2
	int group3 = 277;	// distance from left of pin 1 to left of pin 3

	// m_indexIndex
	m_TopLfRect1 = CRect(left, top, left+width, top+width);
	m_TopCtrRect1 = CRect(left+20, top, left+width+20, top+width);
	m_TopRtRect1 = CRect(left+width+gap, top, left+2*width+gap, top+width);
	m_BotLfRect1 = CRect(left, top+width+gap, left+width, top+2*width+gap);
	m_BotCtrRect1 = CRect(left+20, top+width+gap, left+width+20, top+2*width+gap);
	m_BotRtRect1 = CRect(left+width+gap, top+width+gap, left+2*width+gap, top+2*width+gap);
	m_ThirdLfRect1 = CRect(left, top+2*(width+gap), left+width, top+3*width+2*gap);
	m_ThirdCtrRect1 = CRect(left+20, top+2*(width+gap), left+width+20, top+3*width+2*gap);
	m_ThirdRtRect1 = CRect(left+width+gap, top+2*(width+gap), left+2*width+gap, top+3*width+2*gap);
	
	// m_indexIndex + 1 
	m_TopLfRect2 = CRect(left+group2, top, left+width+group2, top+width);
	m_TopCtrRect2 = CRect(left+20+group2, top, left+width+20+group2, top+width);
	m_TopRtRect2 = CRect(left+width+gap+group2, top, left+2*width+gap+group2, top+width);
	m_BotLfRect2 = CRect(left+group2, top+width+gap, left+width+group2, top+2*width+gap);
	m_BotCtrRect2 = CRect(left+20+group2, top+width+gap, left+width+20+group2, top+2*width+gap);
	m_BotRtRect2 = CRect(left+width+gap+group2, top+width+gap, left+2*width+gap+group2, top+2*width+gap);
	m_ThirdLfRect2 = CRect(left+group2, top+2*(width+gap), left+width+group2, top+3*width+2*gap);
	m_ThirdCtrRect2 = CRect(left+20+group2, top+2*(width+gap), left+width+20+group2, top+3*width+2*gap);
	m_ThirdRtRect2 = CRect(left+width+gap+group2, top+2*(width+gap), left+2*width+gap+group2, top+3*width+2*gap);

	// m_indexIndex + 2
	m_TopLfRect3 = CRect(left+group3, top, left+width+group3, top+width);
	m_TopCtrRect3 = CRect(left+20+group3, top, left+width+20+group3, top+width);
	m_TopRtRect3 = CRect(left+width+gap+group3, top, left+2*width+gap+group3, top+width);
	m_BotLfRect3 = CRect(left+group3, top+width+gap, left+width+group3, top+2*width+gap);
	m_BotCtrRect3 = CRect(left+20+group3, top+width+gap, left+width+20+group3, top+2*width+gap);
	m_BotRtRect3 = CRect(left+width+gap+group3, top+width+gap, left+2*width+gap+group3, top+2*width+gap);
	m_ThirdLfRect3 = CRect(left+group3, top+2*(width+gap), left+width+group3, top+3*width+2*gap);
	m_ThirdCtrRect3 = CRect(left+20+group3, top+2*(width+gap), left+width+20+group3, top+3*width+2*gap);
	m_ThirdRtRect3 = CRect(left+width+gap+group3, top+2*(width+gap), left+2*width+gap+group3, top+3*width+2*gap);

	m_CurveRect1 = CRect(196,343,241,388); // x-22,y+38
	m_CurveRect2 = CRect(437,343,482,388);
	
	m_RedEdit.SetTextColor( RGB(255,0,0) );
	m_RedEdit.SetBkColor( GetSysColor( COLOR_3DFACE ) );
	m_GreenEdit.SetTextColor( RGB(0,255,0) );
	m_GreenEdit.SetBkColor( GetSysColor( COLOR_3DFACE ) );
	m_BlueEdit.SetTextColor( RGB(0,0,255) );
	m_BlueEdit.SetBkColor( GetSysColor( COLOR_3DFACE ) );
	m_red = m_green = m_blue = _T("");

	UpdatePinNumber();
	UpdateCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPinEditDlg::OnPaint()
{
	if(m_nPins < 1)
		return;

	CPaintDC dc(this); // device context for painting

	CPen *pOldPen = (CPen*) dc.SelectStockObject(BLACK_PEN);

	if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::Curve)		// curve graph 1
		DrawCurve(dc, TRUE);

	if(m_nPins > 1 && m_pins.at(m_indexIndex+1).CurveType == ColorCurveType::Curve)	// curve graph 2
		DrawCurve(dc, FALSE);
	
	CBrush brushTop1;
	auto colortop1 = ToColorRef(m_pins.at(m_indexIndex).Color1);
	brushTop1.CreateSolidBrush(colortop1);
	CBrush *pOldBrush = (CBrush*) dc.SelectObject(&brushTop1);

	dc.SelectObject(&brushTop1);
	dc.Rectangle(&m_TopCtrRect1);

	if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
	{
		CBrush brushctr2;
		auto colorBottom = ToColorRef(m_pins.at(m_indexIndex).Color2);
		brushctr2.CreateSolidBrush(colorBottom);
		dc.SelectObject(&brushctr2);
		dc.Rectangle(&m_BotCtrRect1);
	}

	if(m_nPins > 1)
	{
		CBrush brushTop2;
		auto colorTop2 = ToColorRef(m_pins.at(m_indexIndex+1).Color1);
		brushTop2.CreateSolidBrush(colorTop2);
		dc.SelectObject(&brushTop2);
		dc.Rectangle(&m_TopCtrRect2);

		if(m_pins.at(m_indexIndex+1).CurveType == ColorCurveType::DoubleBand || m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
		{
			CBrush brushctr2;
			auto colorBottom = ToColorRef(m_pins.at(m_indexIndex+1).Color2);
			brushctr2.CreateSolidBrush(colorBottom);
			dc.SelectObject(&brushctr2);
			dc.Rectangle(&m_BotCtrRect2);
		}
	}

	if(m_nPins > 2)
	{
		CBrush brushTop3;
		auto colorTop3 = ToColorRef(m_pins.at(m_indexIndex + 2).Color1);
		brushTop3.CreateSolidBrush(colorTop3);
		dc.SelectObject(&brushTop3);
		dc.Rectangle(&m_TopCtrRect3);

		if(m_pins.at(m_indexIndex + 1).CurveType == ColorCurveType::DoubleBand)
		{
			CBrush brushctr2;
			auto colorBottom = ToColorRef(m_pins.at(m_indexIndex + 2).Color2);
			brushctr2.CreateSolidBrush(colorBottom);
			dc.SelectObject(&brushctr2);
			dc.Rectangle(&m_BotCtrRect3);
		}
	}

	// clean up 
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CPinEditDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_nPins < 1)
		return;

	// Group 1
	// TOP CENTER - use left color 
	if(m_TopCtrRect1.PtInRect(point))
	{
		CColorDialog dlg;
		dlg.m_cc.Flags |= CC_FULLOPEN;
		dlg.m_cc.Flags |= CC_RGBINIT;
		
		auto colorTop1 = ToColorRef(m_pins.at(m_indexIndex).Color1);
		dlg.m_cc.lpCustColors = &colorTop1;

		if(dlg.DoModal() == IDOK)
		{
			m_pins.at(m_indexIndex).Color1 = FromColorRef(dlg.GetColor());
			Invalidate(TRUE);
			Dirty();
		}
		return;
	}
		
	if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
	{
		// BOTTOM CENTER - use left color
		if(m_BotCtrRect1.PtInRect(point))
		{
			CColorDialog dlg;
			dlg.m_cc.Flags |= CC_FULLOPEN;
			dlg.m_cc.Flags |= CC_RGBINIT;
			auto colorBottom1 = ToColorRef(m_pins.at(m_indexIndex).Color2);
			dlg.m_cc.lpCustColors = &colorBottom1;
			
			if(dlg.DoModal() == IDOK)
			{
				m_pins.at(m_indexIndex).Color2 = FromColorRef(dlg.GetColor());
				Invalidate(TRUE);
				Dirty();
			}
			return;
		}
	}

	// Group 2
	if(m_nPins > 1)
	{
		if(m_TopCtrRect2.PtInRect(point))
		{
			CColorDialog dlg;
			dlg.m_cc.Flags |= CC_FULLOPEN;
			dlg.m_cc.Flags |= CC_RGBINIT;
			auto colorTop2 = ToColorRef(m_pins.at(m_indexIndex+1).Color1);
			dlg.m_cc.lpCustColors = &colorTop2;

			if(dlg.DoModal() == IDOK)
			{
				m_pins.at(m_indexIndex + 1).Color1 = FromColorRef(dlg.GetColor());
				Invalidate(TRUE);
				Dirty();
			}
			return;
		}

		if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand || m_pins.at(m_indexIndex+1).CurveType == ColorCurveType::DoubleBand)
		{
			if(m_BotCtrRect2.PtInRect(point))
			{
				CColorDialog dlg;
				dlg.m_cc.Flags |= CC_FULLOPEN;
				dlg.m_cc.Flags |= CC_RGBINIT;
				auto colorBottom2 = ToColorRef(m_pins.at(m_indexIndex+1).Color2);
				dlg.m_cc.lpCustColors = &colorBottom2;

				if(dlg.DoModal() == IDOK)
				{
					m_pins.at(m_indexIndex + 1).Color2 = FromColorRef(dlg.GetColor());
					Invalidate(TRUE);
					Dirty();
				}
				return;
			}
		}
	}

	// Group 3
	if(m_nPins > 2)
	{	
		if(m_TopCtrRect3.PtInRect(point))
		{
			CColorDialog dlg;
			dlg.m_cc.Flags |= CC_FULLOPEN;
			dlg.m_cc.Flags |= CC_RGBINIT;
			auto colorTop3 = ToColorRef(m_pins.at(m_indexIndex + 2).Color1);
			dlg.m_cc.lpCustColors = &colorTop3;

			if(dlg.DoModal() == IDOK)
			{
				m_pins.at(m_indexIndex + 2).Color1 = FromColorRef(dlg.GetColor());
				Invalidate(TRUE);
				Dirty();
			}
			return;
		}

		if(m_pins.at(m_indexIndex+2).CurveType == ColorCurveType::DoubleBand || m_pins.at(m_indexIndex + 1).CurveType == ColorCurveType::DoubleBand)
		{
			if(m_BotCtrRect3.PtInRect(point))
			{
				CColorDialog dlg;
				dlg.m_cc.Flags |= CC_FULLOPEN;
				dlg.m_cc.Flags |= CC_RGBINIT;
				auto colorBottom3 = ToColorRef(m_pins.at(m_indexIndex + 2).Color2);
				dlg.m_cc.lpCustColors = &colorBottom3;
					
				if(dlg.DoModal() == IDOK)
				{
					m_pins.at(m_indexIndex + 2).Color2 =FromColorRef(dlg.GetColor());
					Invalidate(TRUE);
					Dirty();
				}
				return;
			}
		}
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPinEditDlg::OnBnClickedNextBut()
{
	if(m_indexIndex < m_nPins - 3)
	{
		m_indexIndex++;
		UpdateCtrls();
	}
}

void CPinEditDlg::OnBnClickedPreviousBut()
{
	if(m_indexIndex > 0)
	{
		m_indexIndex--;
		UpdateCtrls();
	}
}

void CPinEditDlg::OnOK()
{
	CDialogEx::OnOK();
}

void CPinEditDlg::OnCancel()
{
	CDialogEx::OnCancel();
}

void CPinEditDlg::OnBnClickedSpreadNormRad1()
{
	if(m_nPins > 1)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex).CurveType = ColorCurveType::Normal;
		UpdateCtrls();
		Dirty();
	}
}

void CPinEditDlg::OnBnClickedSpreadNormRad2()
{
	if(m_nPins > 2)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex+1).CurveType = ColorCurveType::Normal;
		UpdateCtrls();
		Dirty();
	}
}

void CPinEditDlg::OnBnClickedSpreadStripeRad1()
{
	if(m_nPins > 1)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex).CurveType = ColorCurveType::DoubleBand;
		UpdateCtrls();	
		Dirty();
	}
}

void CPinEditDlg::OnBnClickedSpreadStripeRad2()
{
	if(m_nPins > 2)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex+1).CurveType = ColorCurveType::DoubleBand;
		UpdateCtrls();
		Dirty();
	}	
}

void CPinEditDlg::OnBnClickedSpreadCurveRad1()
{
	if(m_nPins > 1)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex).CurveType = ColorCurveType::Curve;
		UpdateCtrls();	
		Dirty();
	}
}

void CPinEditDlg::OnBnClickedSpreadCurveRad2()
{
	if(m_nPins > 2)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex+1).CurveType = ColorCurveType::Curve;
		UpdateCtrls();
		Dirty();
	}
}

void CPinEditDlg::OnEnKillfocusBandaEdit1()
{
	if(m_nPins > 0)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex).IndexWidth1 = m_BandA1;
		Dirty();
	}
}

void CPinEditDlg::OnEnKillfocusBandaEdit2()
{	
	if(m_nPins > 1)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex+1).IndexWidth1 = m_BandA2;
		Dirty();
	}
}

void CPinEditDlg::OnEnKillfocusBandbEdit1()
{
	if(m_nPins > 0)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex).IndexWidth2 = m_BandB1;
		Dirty();
	}
}

void CPinEditDlg::OnEnKillfocusBandbEdit2()
{
	if(m_nPins > 1)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex+1).IndexWidth2 = m_BandB2;
		Dirty();
	}
}

void CPinEditDlg::OnEnKillfocusCurveEdit1()
{
	if(m_nPins > 0)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex).Curve = m_k1;
		Invalidate(TRUE);
		Dirty();
	}
}

void CPinEditDlg::OnEnKillfocusCurveEdit2()
{
	if(m_nPins > 1)
	{
		UpdateData(TRUE);
		m_pins.at(m_indexIndex+1).Curve = m_k2;
		Invalidate(TRUE);
		Dirty();
	}
}

int CPinEditDlg::CalcPinIndex(int nPin)
{
	int pinPos = nPin - 1;
	return static_cast<int>(m_pins.at(m_indexIndex + pinPos).Index * (m_nColors - 1));
}

double CPinEditDlg::CalcPinIndexValue(int nPin)
{
	int pinIndex = 0;
	if (nPin == 1)
		pinIndex = m_PinIndex1;
	else if (nPin == 2)
		pinIndex = m_PinIndex2;
	else if (pinIndex == 3)
		pinIndex = m_PinIndex3;

	return static_cast<double>(pinIndex) / (m_nColors - 1);
}

void CPinEditDlg::UpdateCtrls()
{
	CWnd *pWnd;

	// do buttons 
	pWnd = GetDlgItem(IDC_PREVIOUS_BUT);
	if(pWnd)
		pWnd->EnableWindow(m_indexIndex > 0);
	pWnd = GetDlgItem(IDC_NEXT_BUT);
	if(pWnd)
		pWnd->EnableWindow(m_indexIndex < m_nPins - 3);
	pWnd = GetDlgItem(IDC_SHOW_BUT);
	if(pWnd)
		pWnd->EnableWindow(m_bDirty);

	CButton *pBut;

	// number of slots filled has to be equal to the number of pins and less than 3
	if(m_nPins > 0)	// first slot will be filled as long as there is one pin
	{
		m_PinNum1 = m_indexIndex + 1; // since m_indexIndex is zero based
		m_PinIndex1 = CalcPinIndex(1);
		m_BandA1 = m_pins.at(m_indexIndex).IndexWidth1;
		if(m_BandA1 < MinBand || m_BandA1 > MaxBand)
			m_BandA1 = DefaultBand;
		m_BandB1 = m_pins.at(m_indexIndex).IndexWidth2;
		if(m_BandB1 < MinBand || m_BandB1 > MaxBand)
			m_BandB1 = DefaultBand;
		m_k1 = m_pins.at(m_indexIndex).Curve;
		if(m_k1 < 0.00001 || m_k1 > 10000)
			m_k1 = 1.0;

		if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
		{
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_NORM_RAD1);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_CURVE_RAD1);
			if(pBut)
				pBut->SetCheck(FALSE);
			
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_STRIPE_RAD1);
			if(pBut)
				pBut->SetCheck(TRUE);
			
			pWnd = GetDlgItem(IDC_CURVE_EDIT1);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
		}
		else if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::Curve)
		{
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_NORM_RAD1);
			if(pBut)
				pBut->SetCheck(FALSE);
			
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_STRIPE_RAD1);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_CURVE_RAD1);
			if(pBut)
				pBut->SetCheck(TRUE);

			pWnd = GetDlgItem(IDC_BANDA_EDIT1);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
			pWnd = GetDlgItem(IDC_BANDB_EDIT1);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
		}
		else if (m_pins.at(m_indexIndex).CurveType == ColorCurveType::Normal)
		{
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_STRIPE_RAD1);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_CURVE_RAD1);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_NORM_RAD1);
			if(pBut)
				pBut->SetCheck(TRUE);

			pWnd = GetDlgItem(IDC_BANDA_EDIT1);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
			pWnd = GetDlgItem(IDC_BANDB_EDIT1);
			if(pWnd)
				pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_CURVE_EDIT1);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
		}	
	}

	if(m_nPins > 1)	// first slot will be filled as long as there is one pin
	{
		m_PinNum2 = m_indexIndex + 2; 
		m_PinIndex2 = CalcPinIndex(2);
		m_BandA2 = m_pins.at(m_indexIndex+1).IndexWidth1;
		if(m_BandA2 < MinBand || m_BandA2 > MaxBand)
			m_BandA2 = DefaultBand;
		m_BandB2 = m_pins.at(m_indexIndex+1).IndexWidth2;
		if(m_BandB2 < MinBand || m_BandB2 > MaxBand)
			m_BandB2 = DefaultBand;
		m_k2 = m_pins.at(m_indexIndex+1).Curve;
		if(m_k2 < 0.00001 || m_k2 > 10000)
			m_k2 = 1.0;

		if(m_pins.at(m_indexIndex+1).CurveType == ColorCurveType::DoubleBand)
		{
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_NORM_RAD2);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_CURVE_RAD2);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_STRIPE_RAD2);
			if(pBut)
				pBut->SetCheck(TRUE);

			pWnd = GetDlgItem(IDC_CURVE_EDIT2);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
		}
		else if(m_pins.at(m_indexIndex + 1).CurveType == ColorCurveType::Curve)
		{
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_NORM_RAD2);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_STRIPE_RAD2);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_CURVE_RAD2);
			if(pBut)
				pBut->SetCheck(TRUE);

			pWnd = GetDlgItem(IDC_BANDA_EDIT2);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
				pWnd = GetDlgItem(IDC_BANDB_EDIT2);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
		}
		else if(m_pins.at(m_indexIndex + 1).CurveType == ColorCurveType::Normal)
		{
			pBut = (CButton*)GetDlgItem(IDC_SPREAD_STRIPE_RAD2);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_CURVE_RAD2);
			if(pBut)
				pBut->SetCheck(FALSE);

			pBut = (CButton*)GetDlgItem(IDC_SPREAD_NORM_RAD2);
			if(pBut)
				pBut->SetCheck(TRUE);

			pWnd = GetDlgItem(IDC_BANDA_EDIT2);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
			pWnd = GetDlgItem(IDC_BANDB_EDIT2);
			if(pWnd)
				pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_CURVE_EDIT2);
			if(pWnd)
				pWnd->EnableWindow(FALSE);
		}
	}

	if(m_nPins > 2)	// first slot will be filled as long as there is one pin
	{
		m_PinNum3 = m_indexIndex + 3; // since m_indexIndex is zero based
		m_PinIndex3 = CalcPinIndex(3);
	}
	
	UpdateData(FALSE);
	Invalidate(TRUE);
}

// if bTop is true use m_rectCurve1
void CPinEditDlg::DrawCurve(CDC &dc, BOOL bTop)
{
	double k = m_k1;	

	CRect *pRect = &m_CurveRect1; // temp as above

	if(! bTop)
	{
		k = m_k2;
		pRect = &m_CurveRect2;
	}

	CPen pen(PS_SOLID,1,RGB(0,0,0));
	CBrush brush(RGB(200,215,220));
	
	CBrush *pOldBrush = dc.SelectObject(&brush);

	dc.Rectangle(pRect);

	CPen *pOldPen = dc.SelectObject(&pen);

	double Xco = 1.0;
	double Xcmax = (double) (pRect->right - pRect->left -1);
	double Yco = 1.0;
	double Ycmax = (double) (pRect->bottom - pRect->top -1);

	double YoutMin = curver(k,Xco);
	double YoutMax = curver(k,Xcmax);

	double y;

	// yes, this can be greatly simplified but its in the same form as used below
	dc.MoveTo(pRect->left+(int)Xco, pRect->top+ (int) (Ycmax-Yco));

	for(double x = Xco+1.0; x <= Xcmax; x++)
	{
		//y = ((log(k*x)-YoutMin)*YoutMax)/(YoutMax-YoutMin);
		y = ((curver(k,x)-YoutMin)*(Ycmax-Yco))/(YoutMax-YoutMin);	// + Yco;

		dc.LineTo(pRect->left+(int)x,  pRect->top + (int)(Ycmax - y ));
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

double CPinEditDlg::curver(double k, double in)
{
	return pow(in,k);
}

// If the control key is pressed then paste m_CopiedColor if m_IsCopiedColor
// else copy the color if the button is in an active rectangle
void CPinEditDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_nPins < 1)
		return;

	// Group 1
	if(m_TopCtrRect1.PtInRect(point))
	{
		if(nFlags & MK_CONTROL)	// paste
		{
			if(m_IsCopiedColor)
			{
				m_pins.at(m_indexIndex).Color1 = FromColorRef(m_CopiedColor);
				Invalidate(TRUE);
				Dirty();
			}
		}
		else	// copy
		{
			m_CopiedColor = ToColorRef(m_pins.at(m_indexIndex).Color1);
			m_IsCopiedColor = TRUE;
		}
	}
		
	// not split but STRIPEs
	if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
	{
		if(m_BotCtrRect1.PtInRect(point))
		{
			if(nFlags & MK_CONTROL)	// paste
			{
				if(m_IsCopiedColor)
				{
					m_pins.at(m_indexIndex).Color2 = FromColorRef(m_CopiedColor);
					Invalidate(TRUE);
					Dirty();
				}
			}
			else	// copy
			{
				m_CopiedColor = ToColorRef(m_pins.at(m_indexIndex).Color2);
				m_IsCopiedColor = TRUE;
			}
		}
	}

	// Group 2
	if(m_nPins > 1)
	{
		if(m_TopCtrRect2.PtInRect(point))
		{
			if(nFlags & MK_CONTROL)	// paste
			{
				if(m_IsCopiedColor)
				{
					m_pins.at(m_indexIndex+1).Color1 = FromColorRef(m_CopiedColor);
					Invalidate(TRUE);
					Dirty();
				}
			}
			else	// copy
			{
				m_CopiedColor = ToColorRef(m_pins.at(m_indexIndex+1).Color1);
				m_IsCopiedColor = TRUE;
			}
		}

		if(m_pins.at(m_indexIndex+1).CurveType == ColorCurveType::DoubleBand || m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
		{
			if(m_BotCtrRect2.PtInRect(point))
			{
				if(nFlags & MK_CONTROL)	// paste
				{
					if(m_IsCopiedColor)
					{
						m_pins.at(m_indexIndex+1).Color2 = FromColorRef(m_CopiedColor);
						Invalidate(TRUE);
						Dirty();
					}
				}
				else	// copy
				{
					m_CopiedColor = ToColorRef(m_pins.at(m_indexIndex + 1).Color2);
					m_IsCopiedColor = TRUE;
				}
			}
		}
	}

	// Group 3
	if(m_nPins > 2)
	{	
		if(m_TopCtrRect3.PtInRect(point))
		{
			if(nFlags & MK_CONTROL)	// paste
			{
				if(m_IsCopiedColor)
				{
					m_pins.at(m_indexIndex + 2).Color1 = FromColorRef(m_CopiedColor);
					Invalidate(TRUE);
					Dirty();
				}
			}
			else	// copy
			{
				m_CopiedColor = ToColorRef(m_pins.at(m_indexIndex + 2).Color1);
				m_IsCopiedColor = TRUE;
			}
		}

		if(m_pins.at(m_indexIndex + 1).CurveType == ColorCurveType::DoubleBand || m_pins.at(m_indexIndex+2).CurveType == ColorCurveType::DoubleBand)
		{
			if(m_BotCtrRect3.PtInRect(point))
			{
				if(nFlags & MK_CONTROL)	// paste
				{
					if(m_IsCopiedColor)
					{
						m_pins.at(m_indexIndex + 2).Color2 = FromColorRef(m_CopiedColor);
						Invalidate(TRUE);
						Dirty();
					}
				}
				else	// copy
				{
					m_CopiedColor = ToColorRef(m_pins.at(m_indexIndex + 2).Color2);
						m_IsCopiedColor = TRUE;
				}
			}
		}
	}
	CDialogEx::OnRButtonUp(nFlags, point);
}

// If the mouse is over a color box, show the RGB values in the edit controls
void CPinEditDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_nPins < 1)
		return;

	m_red = _T("");
	m_green = _T("");
	m_blue = _T("");

	// Group 1
	if(m_TopCtrRect1.PtInRect(point))
	{
		COLORREF color1 = ToColorRef(m_pins.at(m_indexIndex).Color1);
		m_red.Format(_T("%d"), GetRValue(color1));
		m_green.Format(_T("%d"),GetGValue(color1));
		m_blue.Format(_T("%d"), GetBValue(color1));
		UpdateData(FALSE);
		return CDialogEx::OnMouseMove(nFlags, point);
	}

	if(m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
	{
		if(m_BotCtrRect1.PtInRect(point))
		{
			COLORREF color2 = ToColorRef(m_pins.at(m_indexIndex).Color2);
			m_red.Format(_T("%d"), GetRValue(color2));
			m_green.Format(_T("%d"), GetGValue(color2));
			m_blue.Format(_T("%d"), GetBValue(color2));
			UpdateData(FALSE);
			return CDialogEx::OnMouseMove(nFlags, point); 
		}
	}

	// Group 2
	if(m_nPins > 1)
	{
		if(m_TopCtrRect2.PtInRect(point))
		{
			COLORREF color1 = ToColorRef(m_pins.at(m_indexIndex+1).Color1);
			m_red.Format(_T("%d"), GetRValue(color1));
			m_green.Format(_T("%d"), GetGValue(color1));
			m_blue.Format(_T("%d"), GetBValue(color1));
			UpdateData(FALSE);
			return CDialogEx::OnMouseMove(nFlags, point);
		}

		if(m_pins.at(m_indexIndex+1).CurveType == ColorCurveType::DoubleBand || m_pins.at(m_indexIndex).CurveType == ColorCurveType::DoubleBand)
		{
			if(m_BotCtrRect2.PtInRect(point))
			{
				COLORREF color2 = ToColorRef(m_pins.at(m_indexIndex+1).Color2);
				m_red.Format(_T("%d"), GetRValue(color2) );
				m_green.Format(_T("%d"), GetGValue(color2));
				m_blue.Format(_T("%d"), GetBValue(color2));
				UpdateData(FALSE);
				return CDialogEx::OnMouseMove(nFlags, point);
			}
		}
	}

	// Group 3
	if(m_nPins > 2)
	{	
		if(m_TopCtrRect3.PtInRect(point))
		{
			COLORREF color1 = ToColorRef(m_pins.at(m_indexIndex + 2).Color1);
			m_red.Format(_T("%d"), GetRValue(color1));
			m_green.Format(_T("%d"),GetGValue(color1));
			m_blue.Format(_T("%d"), GetBValue(color1));
			UpdateData(FALSE);
			return CDialogEx::OnMouseMove(nFlags, point);
		}

		if(m_pins.at(m_indexIndex + 1).CurveType == ColorCurveType::DoubleBand)
		{
			if(m_BotCtrRect3.PtInRect(point))
			{
				COLORREF color2 = ToColorRef(m_pins.at(m_indexIndex + 2).Color2);
				m_red.Format(_T("%d"), GetRValue(color2) );
				m_green.Format(_T("%d"),GetGValue(color2));
				m_blue.Format(_T("%d"), GetBValue(color2));
				UpdateData(FALSE);
				return CDialogEx::OnMouseMove(nFlags, point);
			}
		}
	}

	UpdateData(FALSE);
	CDialogEx::OnMouseMove(nFlags, point);
}

void CPinEditDlg::OnBnClickedDeleteBut1()
{
	if(m_nPins < 1)
		return;

	CString message;
	message.Format(_T("Are you sure you want to delete pin number %d, at color index %f ?"), m_indexIndex+1, m_PinIndex1);
	if(AfxMessageBox(message, MB_ICONQUESTION + MB_YESNO) == IDYES)
	{
		m_pins.erase(std::begin(m_pins) + m_indexIndex);
		m_nPins = static_cast<int>(m_pins.size());

		m_indexIndex = 0;

		Dirty();
		UpdatePinNumber();
		UpdateCtrls();
		Invalidate(FALSE);
	}
}

void CPinEditDlg::OnBnClickedDeleteBut2()
{
	if(m_nPins < 2)
		return;

	CString message;
	message.Format(_T("Are you sure you want to delete pin number %d, at color index %f ?") , m_indexIndex+2, m_PinIndex2);
	if(AfxMessageBox(message, MB_ICONQUESTION + MB_YESNO) == IDYES)
	{
		m_pins.erase(std::begin(m_pins) + 1 + m_indexIndex);
		m_nPins = static_cast<int>(m_pins.size());

		m_indexIndex = 0;

		Dirty();
		UpdatePinNumber();
		UpdateCtrls();
	}
}

void CPinEditDlg::OnBnClickedDeleteBut3()
{
	if(m_nPins < 3)
		return;

	CString message;
	message.Format(_T("Are you sure you want to delete pin number %d, at color index %f ?") ,m_indexIndex+3, m_PinIndex3);
	if(AfxMessageBox(message, MB_ICONQUESTION + MB_YESNO) == IDYES)
	{
		m_pins.erase(std::begin(m_pins) + 2 + m_indexIndex);
		m_nPins = static_cast<int>(m_pins.size());

		m_indexIndex = 0;

		Dirty();
		UpdatePinNumber();
		UpdateCtrls();
	}
}

// Called by OnInitDialog() and when the number 
// of pins changes (Inserted or deleted)
void CPinEditDlg::UpdatePinNumber()
{
	CWnd *pWnd;

	pWnd = GetDlgItem(IDC_SPREAD_NORM_RAD2);	
	if(pWnd)
		pWnd->EnableWindow(m_nPins>2);

	pWnd = GetDlgItem(IDC_SPREAD_STRIPE_RAD2);	
	if(pWnd)
		pWnd->EnableWindow(m_nPins>2);

	pWnd = GetDlgItem(IDC_SPREAD_CURVE_RAD2);	// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>2);

	pWnd = GetDlgItem(IDC_BANDA_EDIT2);			// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>2);

	pWnd = GetDlgItem(IDC_BANDB_EDIT2);			// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>2);

	pWnd = GetDlgItem(IDC_CURVE_EDIT2);			// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>2);

	pWnd = GetDlgItem(IDC_PIN_NUM_EDIT3);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<3)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_PIN_INDEX_EDIT3);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<3)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_DELETE_BUT3);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<3)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_INSERT_BUT2);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<3)? SW_HIDE : SW_SHOW);

	// Need more than 1 pin
	pWnd = GetDlgItem(IDC_PIN_NUM_EDIT2);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<2)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_PIN_INDEX_EDIT2);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<2)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_DELETE_BUT2);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<2)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_SPREAD_NORM_RAD1);	// disable
	if (pWnd)
		pWnd->EnableWindow(m_nPins > 1);

	pWnd = GetDlgItem(IDC_SPREAD_STRIPE_RAD1);	// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>1);

	pWnd = GetDlgItem(IDC_SPREAD_CURVE_RAD1);	// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>1);

	pWnd = GetDlgItem(IDC_BANDA_EDIT1);			// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>1);

	pWnd = GetDlgItem(IDC_BANDB_EDIT1);			// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>1);

	pWnd = GetDlgItem(IDC_CURVE_EDIT1);			// disable
	if(pWnd)
		pWnd->EnableWindow(m_nPins>1);

	// No Pins
	pWnd = GetDlgItem(IDC_DELETE_BUT1);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<1)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_PIN_NUM_EDIT1);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<1)? SW_HIDE : SW_SHOW);

	pWnd = GetDlgItem(IDC_PIN_INDEX_EDIT1);
	if(pWnd)
		pWnd->ShowWindow((m_nPins<1)? SW_HIDE : SW_SHOW);

}
void CPinEditDlg::OnBnClickedInsertBut1()
{
	if(m_nPins > 0)
	{
		// check m_nColors and calculate a new index
		int newColorIndex;
		int lastIndex = m_PinIndex1;
		if(m_nPins < 2)
		{
			if(lastIndex == m_nColors )
			{
				AfxMessageBox(_T("Sorry, not enough colors to add a new pin after the last!"), MB_ICONWARNING);
				return;
			}
			newColorIndex =  (lastIndex + m_nColors)/2;
		}
		else // more than 1 pin
		{
			int nextIndex = m_PinIndex2;
			if(nextIndex - lastIndex < 2)
			{
				AfxMessageBox(_T("Sorry, not enough colors in between these pins to insert another!"),MB_ICONWARNING);
				return;
			}
			newColorIndex =  (lastIndex + nextIndex)/2;
		}

		ColorPin newPin = m_pins.at(m_indexIndex);
		newPin.Index = static_cast<double>(newColorIndex) / MaxColorIndex;
		m_pins.insert(std::begin(m_pins) + m_indexIndex, newPin);
		m_nPins = static_cast<int>(m_pins.size());
	}	
	else // NO PINS YET so ADD ONE
	{
		ColorPin newPin;
		newPin.Index = 0.0;
		m_pins.push_back(newPin);

		m_nPins = static_cast<int>(m_pins.size());
	}

	Dirty();
	UpdatePinNumber();
	UpdateCtrls();
}

void CPinEditDlg::OnBnClickedInsertBut2()
{
	if(m_nPins < 2)
		return;

	// check m_nColors and calculate a new index
	int newColorIndex;
	int lastIndex = m_PinIndex2;
	if(m_nPins < 3)
	{
		if(lastIndex == m_nColors)
		{
			AfxMessageBox(_T("Sorry, not enough colors to add a new pin after the last!"),MB_ICONWARNING);
			return;
		}
		newColorIndex = (lastIndex + m_nColors)/2;
	}
	else // more than 2 pin
	{
		int nextIndex = m_PinIndex3;
		if(nextIndex - lastIndex < 2)
		{
			AfxMessageBox(_T("Sorry, not enough colors in between these pins to insert another!"),MB_ICONWARNING);
			return;
		}
		newColorIndex = (lastIndex + nextIndex)/2;
	}

	ColorPin newPin = m_pins.at(m_indexIndex+1);
	newPin.Index = static_cast<double>(newColorIndex) / MaxColorIndex;
	m_pins.insert(std::begin(m_pins) + m_indexIndex + 1, newPin);
	m_nPins = static_cast<int>(m_pins.size());

	Dirty();
	UpdatePinNumber();
	UpdateCtrls();
}

// puts them in order of increasing color index
// Using pointer would have been faster but we already started with
// arrays of objects and speed not critical for a dozen or so objects
void CPinEditDlg::SortPins(std::vector<DxColor::ColorPin>& pins)
{
	std::sort(std::begin(m_pins), std::end(m_pins), 
		[&](const ColorPin& lf, const ColorPin& rt)
		{ return lf.Index < rt.Index; });
}

void CPinEditDlg::OnEnKillfocusPinIndexEdit1()
{
	if(m_nPins<1)
		return;

	UpdateData(TRUE);

	if(m_PinIndex1 == CalcPinIndex(1))
		return;		// no change

	if(m_PinIndex1 < 0 || m_PinIndex1 >= m_nColors)
	{
		CString mes;
		mes.Format(_T("Value out of Range! Pick a color index between 0 and %d."), m_nColors-1);
		AfxMessageBox(mes, MB_ICONWARNING);
		m_PinIndex1 = CalcPinIndex(1);
		UpdateData(FALSE);
		return;
	}
	else if( DoesPinAlreadyExist(m_PinIndex1) )
	{
		CString mes;
		mes.Format(_T("Choose another value. A pin already exists with this index: %d."), m_PinIndex1);
		AfxMessageBox(mes, MB_ICONWARNING);
		m_PinIndex1 = CalcPinIndex(1);
		UpdateData(FALSE);
		return;
	}

	m_pins.at(m_indexIndex).Index = CalcPinIndexValue(1);
	SortPins(m_pins);

	UpdatePinNumber();
	UpdateCtrls();
	Dirty();
}

void CPinEditDlg::OnEnKillfocusPinIndexEdit2()
{
	if(m_nPins<2)
		return;

	UpdateData(TRUE);

	if (m_PinIndex2 == CalcPinIndex(2))
		return;

	if(m_PinIndex2 < 0 || m_PinIndex2 >= m_nColors)
	{
		CString mes;
		mes.Format(_T("Value out of Range! Pick a color index between 1 and %d."), m_nColors-1);
		AfxMessageBox(mes, MB_ICONWARNING);
		m_PinIndex2 = CalcPinIndex(2);
		UpdateData(FALSE);
		return;
	}
	else if( DoesPinAlreadyExist(m_PinIndex2) )
	{
		CString mes;
		mes.Format(_T("Choose another value. A pin already exists with this index: %d."),m_PinIndex2);
		AfxMessageBox(mes, MB_ICONWARNING);
		m_PinIndex2 = CalcPinIndex(2);
		UpdateData(FALSE);
		return;
	}

	m_pins.at(m_indexIndex + 1).Index = CalcPinIndexValue(2);
	SortPins(m_pins);

	UpdatePinNumber();
	UpdateCtrls();

	// enable update button
	Dirty();
}

void CPinEditDlg::OnEnKillfocusPinIndexEdit3()
{
	if(m_nPins<3)
		return;

	UpdateData(TRUE);

	if(m_PinIndex3 == CalcPinIndex(3))
		return;		// no change

	if(m_PinIndex3 < 0 || m_PinIndex3 >= m_nColors)
	{
		CString mes;
		mes.Format(_T("Value out of Range! Pick a color index between 1 and %d."), m_nColors - 1);
		AfxMessageBox(mes, MB_ICONWARNING);
		m_PinIndex3 = CalcPinIndex(3);
		UpdateData(FALSE);
		return;
	}
	else if( DoesPinAlreadyExist(m_PinIndex3) )
	{
		CString mes;
		mes.Format(_T("Choose another value. A pin already exists with this index: %d."), m_PinIndex3);
		AfxMessageBox(mes, MB_ICONWARNING);
		m_PinIndex3 = CalcPinIndex(3);
		UpdateData(FALSE);
		return;
	}

	m_pins.at(m_indexIndex + 2).Index = CalcPinIndexValue(3);
	SortPins(m_pins);

	UpdatePinNumber();
	UpdateCtrls();

	// enable update button
	Dirty();
}

// This enables the Update Button
void CPinEditDlg::Dirty(BOOL makeDirty/*=TRUE*/)
{ 
	m_bDirty = makeDirty;

	CWnd *pWnd = GetDlgItem(IDC_SHOW_BUT);
	if(pWnd)
		pWnd->EnableWindow(m_bDirty);
} 

// THESE ON_CHANGE handlers are needed to prevent the lose of the value
// when the focus is lost. Don't know why they are needed. This was not necessary before.
void CPinEditDlg::OnEnChangePinIndexEdit1()
{
	UpdateData(TRUE);
}

void CPinEditDlg::OnEnChangePinIndexEdit2()
{
	UpdateData(TRUE);
}

void CPinEditDlg::OnEnChangePinIndexEdit3()
{
	UpdateData(TRUE);
}

void CPinEditDlg::OnEnChangeBandaEdit1()
{
	UpdateData(TRUE);
	Dirty();
}

void CPinEditDlg::OnEnChangeBandbEdit1()
{
	UpdateData(TRUE);
	Dirty();
}

void CPinEditDlg::OnEnChangeBandaEdit2()
{
	UpdateData(TRUE);
	Dirty();
}

void CPinEditDlg::OnEnChangeBandbEdit2()
{
	UpdateData(TRUE);
	Dirty();
}

void CPinEditDlg::OnEnChangeCurveEdit1()
{
	UpdateData(TRUE);
	Dirty();
}

void CPinEditDlg::OnEnChangeCurveEdit2()
{
	UpdateData(TRUE);
	Dirty();
}

// return true if there is a pin with the same index
bool CPinEditDlg::DoesPinAlreadyExist(int index)
{
	for (const auto& pin : m_pins)
	{
		int pinIndex = static_cast<int>(pin.Index * (MaxColorIndex - 1));
		if (pinIndex == index)
			return true;
	}

	return false;
}

