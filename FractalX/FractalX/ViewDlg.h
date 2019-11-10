#pragma once

#include <Vertex.h>

class CViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewDlg)

public:
	CViewDlg(CWnd* pParent = nullptr);
	~CViewDlg();

	DXF::Vertex<float> GetCamera() const;
	DXF::Vertex<float> GetTarget() const;

	void SetCamera(const DXF::Vertex<float>& camera);
	void SetTarget(const DXF::Vertex<float>& target);

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEW_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnKillfocusEdit();

	DECLARE_MESSAGE_MAP()

	DXF::Vertex<float> m_camera = DXF::Vertex<float>(0.0f, 0.0f, 0.0f);
	DXF::Vertex<float> m_target = DXF::Vertex<float>(0.0f, 0.0f, 0.0f);
};

