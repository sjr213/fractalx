#pragma once

#include "resource.h"
#include "ctrls.h"
#include "afxwin.h"
#include "ColorPin.h"

// notes for FractalX version
// lets pretend the index is an int between 0 and 999, later convert it to double


class CPinEditDlg : public CDialogEx
{
public:
	CPinEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPinEditDlg();

// Dialog Data
	enum { IDD = IDD_PIN_EDIT_DLG };

	void SetPins(std::vector<DxColor::ColorPin>& pins);

	std::vector<DxColor::ColorPin> GetPins() const;

	// This enables the Update Button
	void Dirty(BOOL makeDirty=TRUE);

protected: 

	// maybe add something to set this later
	const int MaxColorIndex = 1000;
	const double MinBand = 0.0;
	const double MaxBand = 1.0;
	const double DefaultBand = 0.005;
	
	int m_nPins;

	std::vector<DxColor::ColorPin> m_pins;

	int m_nBackUpPins;

	std::vector<DxColor::ColorPin> m_backupPins;

	int m_nColors = 1000;	// number of colors 

	int m_indexIndex;	// the index of m_pPin[] in the first position shown
	
	// Special members
	// m_indexIndex
	CRect m_TopLfRect1;
	CRect m_TopCtrRect1;
	CRect m_TopRtRect1;
	CRect m_BotLfRect1;
	CRect m_BotCtrRect1;
	CRect m_BotRtRect1;
	CRect m_ThirdLfRect1;
	CRect m_ThirdCtrRect1;
	CRect m_ThirdRtRect1;

	// m_indexIndex + 1 
	CRect m_TopLfRect2;
	CRect m_TopCtrRect2;
	CRect m_TopRtRect2;
	CRect m_BotLfRect2;
	CRect m_BotCtrRect2;
	CRect m_BotRtRect2;
	CRect m_ThirdLfRect2;
	CRect m_ThirdCtrRect2;
	CRect m_ThirdRtRect2;

	// m_indexIndex + 2
	CRect m_TopLfRect3;
	CRect m_TopCtrRect3;
	CRect m_TopRtRect3;
	CRect m_BotLfRect3;
	CRect m_BotCtrRect3;
	CRect m_BotRtRect3;
	CRect m_ThirdLfRect3;
	CRect m_ThirdCtrRect3;
	CRect m_ThirdRtRect3;

	CRect m_CurveRect1;
	CRect m_CurveRect2;

	// For copy and pasting color
	BOOL m_IsCopiedColor;
	COLORREF m_CopiedColor;

	BOOL m_bDirty;	// whether to enable the update button

	// Control fields
	int m_PinNum1;
	int m_PinNum2;
	int m_PinNum3;

	int m_PinIndex1;
	int m_PinIndex2;
	int m_PinIndex3;

	double m_BandA1;
	double m_BandA2;
	double m_BandB1;
	double m_BandB2;

	double m_k1;
	double m_k2;

	CColorEdit m_RedEdit;
	CColorEdit m_GreenEdit;
	CColorEdit m_BlueEdit;

	CString m_red;
	CString m_green;
	CString m_blue;

	void UpdateCtrls();

	// Called by OnInitDialog() and when the number 
	// of pins changes (Inserted or deleted)
	void UpdatePinNumber();

	void DrawCurve(CDC &dc,BOOL bTop);

	inline double curver(double k, double in);

	// puts them in order of increasing color index
	void SortPins(std::vector<DxColor::ColorPin>& pins);

	// return true if there is a pin with the same index
	bool DoesPinAlreadyExist(int index);

	int CalcPinIndex(int nPin);

	double CalcPinIndexValue(int nPin);

protected:
	
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	virtual void OnOK();

	virtual void OnCancel();

	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedNextBut();
	afx_msg void OnBnClickedPreviousBut();
	afx_msg void OnBnClickedSpreadNormRad1();
	afx_msg void OnBnClickedSpreadNormRad2();
	afx_msg void OnBnClickedSpreadStripeRad1();
	afx_msg void OnBnClickedSpreadStripeRad2();
	afx_msg void OnBnClickedSpreadCurveRad1();
	afx_msg void OnBnClickedSpreadCurveRad2();
	afx_msg void OnEnKillfocusBandaEdit1();
	afx_msg void OnEnKillfocusBandaEdit2();
	afx_msg void OnEnKillfocusBandbEdit1();
	afx_msg void OnEnKillfocusBandbEdit2();
	afx_msg void OnEnKillfocusCurveEdit1();
	afx_msg void OnEnKillfocusCurveEdit2();
	afx_msg void OnBnClickedDeleteBut1();
	afx_msg void OnBnClickedDeleteBut2();
	afx_msg void OnBnClickedDeleteBut3();
	afx_msg void OnBnClickedInsertBut1();
	afx_msg void OnBnClickedInsertBut2();
	afx_msg void OnEnKillfocusPinIndexEdit1();
	afx_msg void OnEnKillfocusPinIndexEdit2();
	afx_msg void OnEnKillfocusPinIndexEdit3();
	afx_msg void OnEnChangePinIndexEdit1();
	afx_msg void OnEnChangePinIndexEdit2();
	afx_msg void OnEnChangePinIndexEdit3();
	afx_msg void OnEnChangeBandaEdit1();
	afx_msg void OnEnChangeBandbEdit1();
	afx_msg void OnEnChangeBandaEdit2();
	afx_msg void OnEnChangeBandbEdit2();
	afx_msg void OnEnChangeCurveEdit1();
	afx_msg void OnEnChangeCurveEdit2();
};
