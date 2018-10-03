#include "stdafx.h"
#include "ContrastDlg.h"

#include <iostream>
#include <sstream>
#include "Resource.h"


using namespace DxColor;

class CContrastDlgImp : public CContrastDlg
{
private:
	DxColor::ColorContrast m_contrast;

	CSliderCtrl m_slider1;
	CSliderCtrl m_slider2;
	CSliderCtrl m_slider3;

	CSliderCtrl m_sliderMax1;
	CSliderCtrl m_sliderMax2;
	CSliderCtrl m_sliderMax3;

public:
	const int Hue_Scale = 10;
	const int Sat_Scale = 1000;
	const int Light_Scale = 1000;

	CContrastDlgImp(const ColorContrast& contrast, CWnd* pParent)
		: CContrastDlg(IDD_PALETTE_VIEW_DLG, pParent)
		, m_contrast(contrast)
	{}

	virtual ~CContrastDlgImp()
	{}

	DxColor::ColorContrast GetContrast() override
	{
		return m_contrast;
	}

	// Dialog Data
	enum { IDD = IDD_CONTRAST_DLG };

protected:
	BOOL OnInitDialog() override
	{
		CContrastDlg::OnInitDialog();

		SetCtrls();

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CContrastDlg::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_SLIDER1, m_slider1);
		DDX_Control(pDX, IDC_SLIDER2, m_slider2);
		DDX_Control(pDX, IDC_SLIDER3, m_slider3);

		DDX_Control(pDX, IDC_SLIDER_MAX1, m_sliderMax1);
		DDX_Control(pDX, IDC_SLIDER_MAX2, m_sliderMax2);
		DDX_Control(pDX, IDC_SLIDER_MAX3, m_sliderMax3);
	}

	DECLARE_MESSAGE_MAP()

	void SetCtrls()
	{
		switch (m_contrast.Mode)
		{
		case ContrastType::Contrast:
			SetCtrlsContrast();
			break;
		case ContrastType::HSL:
			SetCtrlsHSL();
			break;
		default:
			SetCtrlsNone();
		}
	}

	void SetCtrlsNone()
	{
		// 1
		CStatic *static1 = (CStatic*)GetDlgItem(IDC_STATIC_1);
		if (static1)
		{
			static1->ShowWindow(SW_HIDE);
			static1->EnableWindow(FALSE);
			static1->SetWindowTextW(_T(""));
		}

		CSliderCtrl* slider1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
		if (slider1)
		{
			slider1->ShowWindow(SW_HIDE);
			slider1->EnableWindow(FALSE);
		}

		CSliderCtrl* sliderMax1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX1);
		if (sliderMax1)
		{
			sliderMax1->ShowWindow(SW_HIDE);
			sliderMax1->EnableWindow(FALSE);
		}

		CEdit* edit1 = (CEdit*)GetDlgItem(IDC_EDIT1);
		if (edit1)
		{
			edit1->ShowWindow(SW_HIDE);
			edit1->EnableWindow(FALSE);

			edit1->SetWindowTextW(_T(""));
		}

		CEdit* editMax1 = (CEdit*)GetDlgItem(IDC_EDIT_MAX1);
		if (editMax1)
		{
			editMax1->ShowWindow(SW_HIDE);
			editMax1->EnableWindow(FALSE);

			editMax1->SetWindowTextW(_T(""));
		}

		// 2
		CStatic *static2 = (CStatic*)GetDlgItem(IDC_STATIC_2);
		if (static2)
		{
			static2->ShowWindow(SW_HIDE);
			static2->EnableWindow(FALSE);
			static2->SetWindowTextW(_T(""));
		}

		CSliderCtrl* slider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
		if (slider2)
		{
			slider2->ShowWindow(SW_HIDE);
			slider2->EnableWindow(FALSE);
		}

		CSliderCtrl* sliderMax2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX2);
		if (sliderMax2)
		{
			sliderMax2->ShowWindow(SW_HIDE);
			sliderMax2->EnableWindow(FALSE);
		}

		CEdit* edit2 = (CEdit*)GetDlgItem(IDC_EDIT2);
		if (edit2)
		{
			edit2->ShowWindow(SW_HIDE);
			edit2->EnableWindow(FALSE);

			edit2->SetWindowTextW(_T(""));
		}

		CEdit* editMax2 = (CEdit*)GetDlgItem(IDC_EDIT_MAX2);
		if (editMax2)
		{
			editMax2->ShowWindow(SW_HIDE);
			editMax2->EnableWindow(FALSE);

			editMax2->SetWindowTextW(_T(""));
		}

		// 3
		CStatic *static3 = (CStatic*)GetDlgItem(IDC_STATIC_3);
		if (static3)
		{
			static3->ShowWindow(SW_HIDE);
			static3->EnableWindow(FALSE);
			static3->SetWindowTextW(_T(""));
		}

		CSliderCtrl* slider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
		if (slider3)
		{
			slider3->ShowWindow(SW_HIDE);
			slider3->EnableWindow(FALSE);
		}

		CSliderCtrl* sliderMax3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX3);
		if (sliderMax3)
		{
			sliderMax3->ShowWindow(SW_HIDE);
			sliderMax3->EnableWindow(FALSE);
		}

		CEdit* edit3 = (CEdit*)GetDlgItem(IDC_EDIT3);
		if (edit3)
		{
			edit3->ShowWindow(SW_HIDE);
			edit3->EnableWindow(FALSE);

			edit3->SetWindowTextW(_T(""));
		}

		CEdit* editMax3 = (CEdit*)GetDlgItem(IDC_EDIT_MAX3);
		if (editMax3)
		{
			editMax3->ShowWindow(SW_HIDE);
			editMax3->EnableWindow(FALSE);

			editMax3->SetWindowTextW(_T(""));
		}
	}

	void SetCtrlsContrast()
	{
		// Red
		CStatic *static1 = (CStatic*)GetDlgItem(IDC_STATIC_1);
		if (static1)
		{
			static1->ShowWindow(SW_SHOW);
			static1->EnableWindow(TRUE);
			static1->SetWindowTextW(_T("Red"));
		}

		CSliderCtrl* slider1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
		if (slider1)
		{
			slider1->ShowWindow(SW_SHOW);
			slider1->EnableWindow(TRUE);
			slider1->SetRange(0, 255);
			slider1->SetPos(m_contrast.MinContrast[0]);
		}

		CSliderCtrl* sliderMax1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX1);
		if (sliderMax1)
		{
			sliderMax1->ShowWindow(SW_SHOW);
			sliderMax1->EnableWindow(TRUE);
			sliderMax1->SetRange(0, 255);
			sliderMax1->SetPos(m_contrast.MaxContrast[0]);
		}

		CEdit* edit1 = (CEdit*)GetDlgItem(IDC_EDIT1);
		if (edit1)
		{
			edit1->ShowWindow(SW_SHOW);
			edit1->EnableWindow(TRUE);

			std::wstringstream str;
			str << (int)m_contrast.MinContrast[0];
			edit1->SetWindowTextW(str.str().c_str());
		}

		CEdit* editMax1 = (CEdit*)GetDlgItem(IDC_EDIT_MAX1);
		if (editMax1)
		{
			editMax1->ShowWindow(SW_SHOW);
			editMax1->EnableWindow(TRUE);

			std::wstringstream str;
			str << (int)m_contrast.MaxContrast[0];
			editMax1->SetWindowTextW(str.str().c_str());
		}

		// Green
		CStatic *static2 = (CStatic*)GetDlgItem(IDC_STATIC_2);
		if (static2)
		{
			static2->ShowWindow(SW_SHOW);
			static2->EnableWindow(TRUE);
			static2->SetWindowTextW(_T("Green"));
		}

		CSliderCtrl* slider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
		if (slider2)
		{
			slider2->ShowWindow(SW_SHOW);
			slider2->EnableWindow(TRUE);
			slider2->SetRange(0, 255);
			slider2->SetPos(m_contrast.MinContrast[1]);
		}

		CSliderCtrl* sliderMax2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX2);
		if (sliderMax2)
		{
			sliderMax2->ShowWindow(SW_SHOW);
			sliderMax2->EnableWindow(TRUE);
			sliderMax2->SetRange(0, 255);
			sliderMax2->SetPos(m_contrast.MaxContrast[1]);
		}

		CEdit* edit2 = (CEdit*)GetDlgItem(IDC_EDIT2);
		if (edit2)
		{
			edit2->ShowWindow(SW_SHOW);
			edit2->EnableWindow(TRUE);

			std::wstringstream str;
			str << (int)m_contrast.MinContrast[1];
			edit2->SetWindowTextW(str.str().c_str());
		}

		CEdit* editMax2 = (CEdit*)GetDlgItem(IDC_EDIT_MAX2);
		if (editMax2)
		{
			editMax2->ShowWindow(SW_SHOW);
			editMax2->EnableWindow(TRUE);

			std::wstringstream str;
			str << (int)m_contrast.MaxContrast[1];
			editMax2->SetWindowTextW(str.str().c_str());
		}

		// Blue
		CStatic *static3 = (CStatic*)GetDlgItem(IDC_STATIC_3);
		if (static3)
		{
			static3->ShowWindow(SW_SHOW);
			static3->EnableWindow(TRUE);
			static3->SetWindowTextW(_T("Blue"));
		}

		CSliderCtrl* slider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
		if (slider3)
		{
			slider3->ShowWindow(SW_SHOW);
			slider3->EnableWindow(TRUE);
			slider3->SetRange(0, 255);
			slider3->SetPos(m_contrast.MinContrast[2]);
		}

		CSliderCtrl* sliderMax3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX3);
		if (sliderMax3)
		{
			sliderMax3->ShowWindow(SW_SHOW);
			sliderMax3->EnableWindow(TRUE);
			sliderMax3->SetRange(0, 255);
			sliderMax3->SetPos(m_contrast.MaxContrast[2]);
		}

		CEdit* edit3 = (CEdit*)GetDlgItem(IDC_EDIT3);
		if (edit3)
		{
			edit3->ShowWindow(SW_SHOW);
			edit3->EnableWindow(TRUE);

			std::wstringstream str;
			str << (int)m_contrast.MinContrast[2];
			edit3->SetWindowTextW(str.str().c_str());
		}

		CEdit* editMax3 = (CEdit*)GetDlgItem(IDC_EDIT_MAX3);
		if (editMax3)
		{
			editMax3->ShowWindow(SW_SHOW);
			editMax3->EnableWindow(TRUE);

			std::wstringstream str;
			str << (int)m_contrast.MaxContrast[2];
			editMax3->SetWindowTextW(str.str().c_str());
		}
	}

	void SetCtrlsHSL()
	{
		// Hue
		CStatic *static1 = (CStatic*)GetDlgItem(IDC_STATIC_1);
		if (static1)
		{
			static1->ShowWindow(SW_SHOW);
			static1->EnableWindow(TRUE);
			static1->SetWindowTextW(_T("Hue"));
		}

		CSliderCtrl* slider1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
		if (slider1)
		{
			slider1->ShowWindow(SW_SHOW);
			slider1->EnableWindow(TRUE);
			slider1->SetRange(0, static_cast<int>(Hue_Scale * MAX_HUE));
			slider1->SetPos(static_cast<int>(Hue_Scale * m_contrast.MinHue));
		}

		CSliderCtrl* sliderMax1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX1);
		if (sliderMax1)
		{
			sliderMax1->ShowWindow(SW_SHOW);
			sliderMax1->EnableWindow(TRUE);
			sliderMax1->SetRange(0, static_cast<int>(Hue_Scale * MAX_HUE));
			sliderMax1->SetPos(static_cast<int>(Hue_Scale * m_contrast.MaxHue));
		}

		CEdit* edit1 = (CEdit*)GetDlgItem(IDC_EDIT1);
		if (edit1)
		{
			edit1->ShowWindow(SW_SHOW);
			edit1->EnableWindow(TRUE);

			std::wstringstream str;
			str << static_cast<int>(Hue_Scale * m_contrast.MinHue);
			edit1->SetWindowTextW(str.str().c_str());
		}

		CEdit* editMax1 = (CEdit*)GetDlgItem(IDC_EDIT_MAX1);
		if (editMax1)
		{
			editMax1->ShowWindow(SW_SHOW);
			editMax1->EnableWindow(TRUE);

			std::wstringstream str;
			str << static_cast<int>(Hue_Scale * m_contrast.MaxHue);
			editMax1->SetWindowTextW(str.str().c_str());
		}

		// Saturation
		CStatic *static2 = (CStatic*)GetDlgItem(IDC_STATIC_2);
		if (static2)
		{
			static2->ShowWindow(SW_SHOW);
			static2->EnableWindow(TRUE);
			static2->SetWindowTextW(_T("Saturation"));
		}

		CSliderCtrl* slider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
		if (slider2)
		{
			slider2->ShowWindow(SW_SHOW);
			slider2->EnableWindow(TRUE);
			slider2->SetRange(0, static_cast<int>(Sat_Scale * MAX_SATURATION));
			slider2->SetPos(static_cast<int>(Sat_Scale * m_contrast.MinSaturation));
		}

		CSliderCtrl* sliderMax2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX2);
		if (sliderMax2)
		{
			sliderMax2->ShowWindow(SW_SHOW);
			sliderMax2->EnableWindow(TRUE);
			sliderMax2->SetRange(0, static_cast<int>(Sat_Scale * MAX_SATURATION));
			sliderMax2->SetPos(static_cast<int>(Sat_Scale * m_contrast.MaxSaturation));
		}

		CEdit* edit2 = (CEdit*)GetDlgItem(IDC_EDIT2);
		if (edit2)
		{
			edit2->ShowWindow(SW_SHOW);
			edit2->EnableWindow(TRUE);

			std::wstringstream str;
			str << static_cast<int>(Sat_Scale * m_contrast.MinSaturation);
			edit2->SetWindowTextW(str.str().c_str());
		}

		CEdit* editMax2 = (CEdit*)GetDlgItem(IDC_EDIT_MAX2);
		if (editMax2)
		{
			editMax2->ShowWindow(SW_SHOW);
			editMax2->EnableWindow(TRUE);

			std::wstringstream str;
			str << static_cast<int>(Sat_Scale * m_contrast.MaxSaturation);
			editMax2->SetWindowTextW(str.str().c_str());
		}

		// Lightness
		CStatic *static3 = (CStatic*)GetDlgItem(IDC_STATIC_3);
		if (static3)
		{
			static3->ShowWindow(SW_SHOW);
			static3->EnableWindow(TRUE);
			static3->SetWindowTextW(_T("Lightness"));
		}

		CSliderCtrl* slider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
		if (slider3)
		{
			slider3->ShowWindow(SW_SHOW);
			slider3->EnableWindow(TRUE);
			slider3->SetRange(0, static_cast<int>(Light_Scale * MAX_LIGHTNESS));
			slider3->SetPos(static_cast<int>(Sat_Scale * m_contrast.MinLightness));
		}

		CSliderCtrl* sliderMax3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX3);
		if (sliderMax3)
		{
			sliderMax3->ShowWindow(SW_SHOW);
			sliderMax3->EnableWindow(TRUE);
			sliderMax3->SetRange(0, static_cast<int>(Light_Scale * MAX_LIGHTNESS));
			sliderMax3->SetPos(static_cast<int>(Sat_Scale * m_contrast.MaxLightness));
		}

		CEdit* edit3 = (CEdit*)GetDlgItem(IDC_EDIT3);
		if (edit3)
		{
			edit3->ShowWindow(SW_SHOW);
			edit3->EnableWindow(TRUE);

			std::wstringstream str;
			str << static_cast<int>(Sat_Scale * m_contrast.MinLightness);
			edit3->SetWindowTextW(str.str().c_str());
		}

		CEdit* editMax3 = (CEdit*)GetDlgItem(IDC_EDIT_MAX3);
		if (editMax3)
		{
			editMax3->ShowWindow(SW_SHOW);
			editMax3->EnableWindow(TRUE);

			std::wstringstream str;
			str << static_cast<int>(Sat_Scale * m_contrast.MaxLightness);
			editMax3->SetWindowTextW(str.str().c_str());
		}
	}

	void  OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* pScroll)
	{
		/*
		auto pSlider = reinterpret_cast<CSliderCtrl*>(pScroll);
		if (pSlider != &m_wndMinSlider && pSlider != &m_wndMaxSlider)
			return;

		int pos = pSlider->GetPos();
		if (pSlider == &m_wndMinSlider)
		{
			m_eMinBubbleRadius = (double)pos / 100;

			if (m_eMinBubbleRadius > m_eMaxBubbleRadius)
			{
				m_eMaxBubbleRadius = m_eMinBubbleRadius;
				m_wndMaxSlider.SetPos(static_cast<int>(m_eMaxBubbleRadius * 100));
			}
		}

		if (pSlider == &m_wndMaxSlider)
		{
			m_eMaxBubbleRadius = (double)pos / 100;

			if (m_eMaxBubbleRadius < m_eMinBubbleRadius)
			{
				m_eMinBubbleRadius = m_eMaxBubbleRadius;
				m_wndMinSlider.SetPos(static_cast<int>(m_eMaxBubbleRadius * 100));
			}
		}
		*/
		UpdateData(FALSE);
	}

};

BEGIN_MESSAGE_MAP(CContrastDlgImp, CContrastDlg)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

std::shared_ptr<CContrastDlg> CContrastDlg::CreateContrastDlg(const DxColor::ColorContrast& contrast, CWnd* pParent)
{
	return std::make_shared <CContrastDlgImp>(contrast, pParent);
}