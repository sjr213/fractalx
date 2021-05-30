#include "stdafx.h"
#include "ContrastDlg.h"

#include <iostream>
#include "NarrowCast.h"
#include <sstream>
#include "Resource.h"


using namespace DxColor;

class CContrastDlgImp : public CContrastDlg
{
private:
	DxColor::ColorContrast m_contrast;

	CComboBox m_ModeCombo;

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
		: CContrastDlg(IDD_CONTRAST_DLG, pParent)
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

		InitializeModeCombo();

		SetCtrls();

		return TRUE;
	}

	void InitializeModeCombo()
	{
		auto pCombo = (CComboBox*)GetDlgItem(IDC_MODE_COMBO);
		if (!pCombo)
			return;

		pCombo->InsertString(0, _T("None"));
		pCombo->InsertString(1, _T("Contrast"));
		pCombo->InsertString(2, _T("HSL"));

		pCombo->SetCurSel(static_cast<int>(m_contrast.Mode));
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CContrastDlg::DoDataExchange(pDX);

		int nMode = static_cast<int>(m_contrast.Mode);
		DDX_Control(pDX, IDC_MODE_COMBO, m_ModeCombo);
		DDX_CBIndex(pDX, IDC_MODE_COMBO, nMode);
		m_contrast.Mode = static_cast<ContrastType>(nMode);

		DDX_Control(pDX, IDC_SLIDER1, m_slider1);
		DDX_Control(pDX, IDC_SLIDER2, m_slider2);
		DDX_Control(pDX, IDC_SLIDER3, m_slider3);

		DDX_Control(pDX, IDC_SLIDER_MAX1, m_sliderMax1);
		DDX_Control(pDX, IDC_SLIDER_MAX2, m_sliderMax2);
		DDX_Control(pDX, IDC_SLIDER_MAX3, m_sliderMax3);

		if (m_contrast.Mode == ContrastType::Contrast)
		{
			int minRed = m_contrast.MinContrast[0];
			DDX_Text(pDX, IDC_EDIT1, minRed);
			DDV_MinMaxInt(pDX, minRed, 0, 255);
			m_contrast.MinContrast[0] = DxCore::narrow_cast<bite>(minRed);

			int maxRed = m_contrast.MaxContrast[0];
			DDX_Text(pDX, IDC_EDIT_MAX1, maxRed);
			DDV_MinMaxInt(pDX, maxRed, 0, 255);
			m_contrast.MaxContrast[0] = DxCore::narrow_cast<bite>(maxRed);

			int minGreen = m_contrast.MinContrast[1];
			DDX_Text(pDX, IDC_EDIT2, minGreen);
			DDV_MinMaxInt(pDX, minGreen, 0, 255);
			m_contrast.MinContrast[1] = DxCore::narrow_cast<bite>(minGreen);

			int maxGreen = m_contrast.MaxContrast[1];
			DDX_Text(pDX, IDC_EDIT_MAX2, maxGreen);
			DDV_MinMaxInt(pDX, maxGreen, 0, 255);
			m_contrast.MaxContrast[1] = DxCore::narrow_cast<bite>(maxGreen);

			int minBlue = m_contrast.MinContrast[2];
			DDX_Text(pDX, IDC_EDIT3, minBlue);
			DDV_MinMaxInt(pDX, minBlue, 0, 255);
			m_contrast.MinContrast[2] = DxCore::narrow_cast<bite>(minBlue);

			int maxBlue = m_contrast.MaxContrast[2];
			DDX_Text(pDX, IDC_EDIT_MAX3, maxBlue);
			DDV_MinMaxInt(pDX, maxBlue, 0, 255);
			m_contrast.MaxContrast[2] = DxCore::narrow_cast<bite>(maxBlue);
		}
		else if (m_contrast.Mode == ContrastType::HSL)
		{
			double minHue = m_contrast.MinHue;
			DDX_Text(pDX, IDC_EDIT1, minHue);
			DDV_MinMaxDouble(pDX, minHue, 0, MAX_HUE);
			m_contrast.MinHue = minHue;

			double maxHue = m_contrast.MaxHue;
			DDX_Text(pDX, IDC_EDIT_MAX1, maxHue);
			DDV_MinMaxDouble(pDX, maxHue, 0, MAX_HUE);
			m_contrast.MaxHue = maxHue;

			double minSat = m_contrast.MinSaturation;
			DDX_Text(pDX, IDC_EDIT2, minSat);
			DDV_MinMaxDouble(pDX, minSat, 0, MAX_SATURATION);
			m_contrast.MinSaturation = minSat;

			double maxSat = m_contrast.MaxSaturation;
			DDX_Text(pDX, IDC_EDIT_MAX2, maxSat);
			DDV_MinMaxDouble(pDX, maxSat, 0, MAX_SATURATION);
			m_contrast.MaxSaturation = maxSat;

			double minLight = m_contrast.MinLightness;
			DDX_Text(pDX, IDC_EDIT3, minLight);
			DDV_MinMaxDouble(pDX, minLight, 0, MAX_LIGHTNESS);
			m_contrast.MinLightness = minLight;

			double maxLight = m_contrast.MaxLightness;
			DDX_Text(pDX, IDC_EDIT_MAX3, maxLight);
			DDV_MinMaxDouble(pDX, maxLight, 0, MAX_LIGHTNESS);
			m_contrast.MaxLightness = maxLight;
		}
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

	void OnHScrollContrast(CScrollBar* pScroll)
	{
		auto pSlider = reinterpret_cast<CSliderCtrl*>(pScroll);
		if (!pSlider)
			return;

		int pos = pSlider->GetPos();

		if (pSlider == &m_slider1)
		{
			m_contrast.MinContrast[0] = DxCore::narrow_cast<bite>(pos);

			if (m_contrast.MinContrast[0] > m_contrast.MaxContrast[0])
			{
				m_contrast.MaxContrast[0] = DxCore::narrow_cast<bite>(pos);
				m_sliderMax1.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_sliderMax1)
		{
			m_contrast.MaxContrast[0] = DxCore::narrow_cast<bite>(pos);

			if (m_contrast.MaxContrast[0] < m_contrast.MinContrast[0])
			{
				m_contrast.MinContrast[0] = DxCore::narrow_cast<bite>(pos);
				m_slider1.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_slider2)
		{
			m_contrast.MinContrast[1] = DxCore::narrow_cast<bite>(pos);

			if (m_contrast.MinContrast[1] > m_contrast.MaxContrast[1])
			{
				m_contrast.MaxContrast[1] = DxCore::narrow_cast<bite>(pos);
				m_sliderMax2.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_sliderMax2)
		{
			m_contrast.MaxContrast[1] = DxCore::narrow_cast<bite>(pos);

			if (m_contrast.MaxContrast[1] < m_contrast.MinContrast[1])
			{
				m_contrast.MinContrast[1] = DxCore::narrow_cast<bite>(pos);
				m_slider2.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_slider3)
		{
			m_contrast.MinContrast[2] = DxCore::narrow_cast<bite>(pos);

			if (m_contrast.MinContrast[2] > m_contrast.MaxContrast[2])
			{
				m_contrast.MaxContrast[2] = DxCore::narrow_cast<bite>(pos);
				m_sliderMax3.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_sliderMax3)
		{
			m_contrast.MaxContrast[2] = DxCore::narrow_cast<bite>(pos);

			if (m_contrast.MaxContrast[2] < m_contrast.MinContrast[2])
			{
				m_contrast.MinContrast[2] = DxCore::narrow_cast<bite>(pos);
				m_slider3.SetPos(pos);
			}
			return;
		}
	}

	void OnHScrollHSL(CScrollBar* pScroll)
	{
		auto pSlider = reinterpret_cast<CSliderCtrl*>(pScroll);
		if (!pSlider)
			return;

		int pos = pSlider->GetPos();

		if (pSlider == &m_slider1)
		{
			m_contrast.MinHue = static_cast<double>(pos) / Hue_Scale;

			if (m_contrast.MinHue > m_contrast.MaxHue)
			{
				m_contrast.MaxHue = m_contrast.MinHue;
				m_sliderMax1.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_sliderMax1)
		{
			m_contrast.MaxHue = static_cast<double>(pos) / Hue_Scale;

			if (m_contrast.MaxHue < m_contrast.MinHue)
			{
				m_contrast.MinHue = m_contrast.MaxHue;
				m_slider1.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_slider2)
		{
			m_contrast.MinSaturation = static_cast<double>(pos) / Sat_Scale;

			if (m_contrast.MinSaturation > m_contrast.MaxSaturation)
			{
				m_contrast.MaxSaturation = m_contrast.MinSaturation;
				m_sliderMax2.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_sliderMax2)
		{
			m_contrast.MaxSaturation = static_cast<double>(pos) / Sat_Scale;

			if (m_contrast.MaxSaturation < m_contrast.MinSaturation)
			{
				m_contrast.MinSaturation = m_contrast.MaxSaturation;
				m_slider2.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_slider3)
		{
			m_contrast.MinLightness = static_cast<double>(pos) / Light_Scale;

			if (m_contrast.MinLightness > m_contrast.MaxLightness)
			{
				m_contrast.MaxLightness = m_contrast.MinLightness;
				m_sliderMax3.SetPos(pos);
			}
			return;
		}

		if (pSlider == &m_sliderMax3)
		{
			m_contrast.MaxLightness = static_cast<double>(pos) / Light_Scale;

			if (m_contrast.MaxLightness < m_contrast.MinLightness)
			{
				m_contrast.MinLightness = m_contrast.MaxLightness;
				m_slider3.SetPos(pos);
			}
			return;
		}
	}

	void  OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* pScroll)
	{
		switch (m_contrast.Mode)
		{
		case ContrastType::Contrast:
			OnHScrollContrast(pScroll);
			break;
		case ContrastType::HSL:
			OnHScrollHSL(pScroll);
			break;
		}

		UpdateData(FALSE);
	}

	void OnModeChanged()
	{
		auto pCombo = (CComboBox*)GetDlgItem(IDC_MODE_COMBO);
		if (!pCombo)
			return;
		int nMode = pCombo->GetCurSel();
		m_contrast.Mode = static_cast<ContrastType>(nMode);
		
		SetCtrls();

		UpdateData(FALSE);
	}

	void UpdateSliders()
	{
		switch (m_contrast.Mode)
		{
		case ContrastType::Contrast:
			UpdateSlidersContrast();
			break;
		case ContrastType::HSL:
			UpdateSlidersHSL();
			break;
		}
	}

	void UpdateSlidersContrast()
	{
		CSliderCtrl* slider1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
		if (slider1)
			slider1->SetPos(m_contrast.MinContrast[0]);

		CSliderCtrl* sliderMax1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX1);
		if (sliderMax1)
			sliderMax1->SetPos(m_contrast.MaxContrast[0]);

		CSliderCtrl* slider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
		if (slider2)
			slider2->SetPos(m_contrast.MinContrast[1]);

		CSliderCtrl* sliderMax2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX2);
		if (sliderMax2)
			sliderMax2->SetPos(m_contrast.MaxContrast[1]);

		CSliderCtrl* slider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
		if (slider3)
			slider3->SetPos(m_contrast.MinContrast[2]);

		CSliderCtrl* sliderMax3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX3);
			sliderMax3->SetPos(m_contrast.MaxContrast[2]);
	}

	void UpdateSlidersHSL()
	{
		CSliderCtrl* slider1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
		if (slider1)
			slider1->SetPos(static_cast<int>(Hue_Scale * m_contrast.MinHue));

		CSliderCtrl* sliderMax1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX1);
		if (sliderMax1)
			sliderMax1->SetPos(static_cast<int>(Hue_Scale * m_contrast.MaxHue));

		CSliderCtrl* slider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
		if (slider2)
			slider2->SetPos(static_cast<int>(Sat_Scale * m_contrast.MinSaturation));

		CSliderCtrl* sliderMax2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX2);
		if (sliderMax2)
			sliderMax2->SetPos(static_cast<int>(Sat_Scale * m_contrast.MaxSaturation));

		CSliderCtrl* slider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
		if (slider3)
			slider3->SetPos(static_cast<int>(Sat_Scale * m_contrast.MinLightness));

		CSliderCtrl* sliderMax3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MAX3);
		if (sliderMax3)
			sliderMax3->SetPos(static_cast<int>(Sat_Scale * m_contrast.MaxLightness));
	}

	void OnKillFocusMinEdit()
	{
		UpdateData(TRUE);

		bool change = false;
		if (m_contrast.Mode == ContrastType::Contrast)
		{
			if (m_contrast.MaxContrast[0] < m_contrast.MinContrast[0])
			{
				m_contrast.MaxContrast[0] = m_contrast.MinContrast[0];
				change = true;
			}

			if (m_contrast.MaxContrast[1] < m_contrast.MinContrast[1])
			{
				m_contrast.MaxContrast[1] = m_contrast.MinContrast[1];
				change = true;
			}

			if (m_contrast.MaxContrast[2] < m_contrast.MinContrast[2])
			{
				m_contrast.MaxContrast[2] = m_contrast.MinContrast[2];
				change = true;
			}
		}
		else if (m_contrast.Mode == ContrastType::HSL)
		{
			if (m_contrast.MaxHue < m_contrast.MinHue)
			{
				m_contrast.MaxHue = m_contrast.MinHue;
				change = true;
			}

			if (m_contrast.MaxSaturation < m_contrast.MinSaturation)
			{
				m_contrast.MaxSaturation = m_contrast.MinSaturation;
				change = true;
			}

			if (m_contrast.MaxLightness < m_contrast.MinLightness)
			{
				m_contrast.MaxLightness = m_contrast.MinLightness;
				change = true;
			}
		}

		if (change)
			UpdateData(FALSE);

		UpdateSliders();
	}

	void OnKillFocusMaxEdit()
	{
		UpdateData(TRUE);

		bool change = false;
		if (m_contrast.Mode == ContrastType::Contrast)
		{
			if (m_contrast.MaxContrast[0] < m_contrast.MinContrast[0])
			{
				m_contrast.MinContrast[0] = m_contrast.MaxContrast[0];
				change = true;
			}

			if (m_contrast.MaxContrast[1] < m_contrast.MinContrast[1])
			{
				m_contrast.MinContrast[1] = m_contrast.MaxContrast[1];
				change = true;
			}

			if (m_contrast.MaxContrast[2] < m_contrast.MinContrast[2])
			{
				m_contrast.MinContrast[2] = m_contrast.MaxContrast[2];
				change = true;
			}
		}
		else if (m_contrast.Mode == ContrastType::HSL)
		{
			if (m_contrast.MaxHue < m_contrast.MinHue)
			{
				m_contrast.MinHue = m_contrast.MaxHue;
				change = true;
			}

			if (m_contrast.MaxSaturation < m_contrast.MinSaturation)
			{
				m_contrast.MinSaturation = m_contrast.MaxSaturation;
				change = true;
			}

			if (m_contrast.MaxLightness < m_contrast.MinLightness)
			{
				m_contrast.MinLightness = m_contrast.MaxLightness;
				change = true;
			}
		}

		if (change)
			UpdateData(FALSE);

		UpdateSliders();
	}
};

BEGIN_MESSAGE_MAP(CContrastDlgImp, CContrastDlg)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_MODE_COMBO, &CContrastDlgImp::OnModeChanged)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CContrastDlgImp::OnKillFocusMinEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX1, &CContrastDlgImp::OnKillFocusMaxEdit)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CContrastDlgImp::OnKillFocusMinEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX2, &CContrastDlgImp::OnKillFocusMaxEdit)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CContrastDlgImp::OnKillFocusMinEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX3, &CContrastDlgImp::OnKillFocusMaxEdit)
END_MESSAGE_MAP()

std::shared_ptr<CContrastDlg> CContrastDlg::CreateContrastDlg(const DxColor::ColorContrast& contrast, CWnd* pParent)
{
	return std::make_shared <CContrastDlgImp>(contrast, pParent);
}