#pragma once

// This is no longer used - delete if there is a problem

#include "resource.h"       // main symbols
#include <afxwin.h>
#include <afxdialogex.h>

class CTextureDlg : public CDialogEx
{
public:
	CTextureDlg();
	~CTextureDlg();

	int GetTextureId() const;

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_TEXTURE_DLG };
#endif

protected:
		virtual void DoDataExchange(CDataExchange* pDX);

protected:
		DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

private:
	int m_selectedTextureId;
	CListBox m_textureList;
};
