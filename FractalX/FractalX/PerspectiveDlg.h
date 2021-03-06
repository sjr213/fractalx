#pragma once


// CPerspectiveDlg dialog

class CPerspectiveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPerspectiveDlg)

public:
	CPerspectiveDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPerspectiveDlg();

	void SetNear(float nearDistance);
	void SetFarDistance(float farDistance);

	float GetNearDistance() const;
	float GetFarDistance() const;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PERSPECTIVE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnKillfocusNearEdit();
	afx_msg void OnKillfocusFarEdit();

	DECLARE_MESSAGE_MAP()
	float m_nearDistance;
	float m_farDistance;
};
