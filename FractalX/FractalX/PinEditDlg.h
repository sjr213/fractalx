#pragma once

#include "resource.h"
#include "ctrls.h"
#include "afxwin.h"

class CPinn;
// CPinEditDlg dialog

class AFX_EXT_CLASS CPinEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CPinEditDlg)

public:
	CPinEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPinEditDlg();

// Dialog Data
	enum { IDD = IDD_PIN_EDIT_DLG };

	int m_nPins;
	CPinn *m_pPins;
	int m_nBackUpPins;
	CPinn *m_pBackUpPins;
	
	int m_nColors;	// number of colors ( Maximum pin index is m_nColors -1)
	RGBQUAD* m_pColors;
	RGBQUAD* m_pBackUpColors;

	int m_indexIndex;	// the index of m_pPin[] in the first position shown

	// This enables the Update Button
	void Dirty(BOOL makeDirty=TRUE);

protected: // Special members
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
	BOOL m_bParentChanged;	// true if an update was sent to the parent
							// means OnOK, a backup can be made even if !m_bDirty

	void UpdateCtrls();

	// Called by OnInitDialog() and when the number 
	// of pins changes (Inserted or deleted)
	void UpdatePinNumber();

	void DrawCurve(CDC &dc,BOOL bTop);

	inline double curver(double k, double in);

	// puts them in order of increasing color index
	void SortPins(int count, CPinn *pPins);

	// return true if there is a pin with the same index
	BOOL DoesPinAlreadyExist(int index);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_Connect2;
public:
	BOOL m_Connect1;
public:
	int m_PinNum1;
public:
	int m_PinNum2;
public:
	int m_PinNum3;
public:
	int m_PinIndex1;
public:
	int m_PinIndex2;
public:
	int m_PinIndex3;
public:
	int m_BandA1;
public:
	int m_BandA2;
public:
	int m_BandB1;
public:
	int m_BandB2;
public:
	double m_k1;
public:
	double m_k2;
public:
	BOOL m_Split1;
public:
	BOOL m_Split2;
public:
	BOOL m_Split3;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnBnClickedNextBut();
public:
	afx_msg void OnBnClickedPreviousBut();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedConnectCheck1();
public:
	afx_msg void OnBnClickedConnectCheck2();
public:
	afx_msg void OnBnClickedSpreadNormRad1();
public:
	afx_msg void OnBnClickedSpreadNormRad2();
public:
	afx_msg void OnBnClickedSpreadStripeRad1();
public:
	afx_msg void OnBnClickedSpreadStripeRad2();
public:
	afx_msg void OnBnClickedSpreadCurveRad1();
public:
	afx_msg void OnBnClickedSpreadCurveRad2();
public:
	afx_msg void OnEnKillfocusBandaEdit1();
public:
	afx_msg void OnEnKillfocusBandaEdit2();
public:
	afx_msg void OnEnKillfocusBandbEdit1();
public:
	afx_msg void OnEnKillfocusBandbEdit2();
public:
	afx_msg void OnEnKillfocusCurveEdit1();
public:
	afx_msg void OnEnKillfocusCurveEdit2();
public:
	afx_msg void OnBnClickedShowBut();
public:
	afx_msg void OnBnClickedSplitCheck1();
public:
	afx_msg void OnBnClickedSplitCheck2();
public:
	afx_msg void OnBnClickedSplitCheck3();
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
public:
	CColorEdit m_RedEdit;
public:
	CColorEdit m_GreenEdit;
public:
	CColorEdit m_BlueEdit;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	CString m_red;
public:
	CString m_green;
public:
	CString m_blue;
public:
	afx_msg void OnBnClickedDeleteBut1();
public:
	afx_msg void OnBnClickedDeleteBut2();
public:
	afx_msg void OnBnClickedDeleteBut3();
public:
	afx_msg void OnBnClickedInsertBut1();
public:
	afx_msg void OnBnClickedInsertBut2();
public:
	afx_msg void OnEnKillfocusPinIndexEdit1();
public:
	afx_msg void OnEnKillfocusPinIndexEdit2();
public:
	afx_msg void OnEnKillfocusPinIndexEdit3();
public:
	afx_msg void OnEnChangePinIndexEdit1();
public:
	afx_msg void OnEnChangePinIndexEdit2();
public:
	afx_msg void OnEnChangePinIndexEdit3();
public:
	afx_msg void OnEnChangeBandaEdit1();
public:
	afx_msg void OnEnChangeBandbEdit1();
public:
	afx_msg void OnEnChangeBandaEdit2();
public:
	afx_msg void OnEnChangeBandbEdit2();
public:
	afx_msg void OnEnChangeCurveEdit1();
public:
	afx_msg void OnEnChangeCurveEdit2();
public:
	int m_BandC1;
public:
	int m_BandC2;
public:
	afx_msg void OnEnChangeBandcEdit1();
public:
	afx_msg void OnEnKillfocusBandcEdit1();
public:
	afx_msg void OnEnChangeBandcEdit2();
public:
	afx_msg void OnEnKillfocusBandcEdit2();
};
