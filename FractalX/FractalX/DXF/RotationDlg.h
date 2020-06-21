#pragma once

#include "resource.h"       // main symbols
#include <afxwin.h>
#include "RotationGroup.h"

namespace DXF
{
	class CRotationDlg : public CDialogEx
	{
	public:
		CRotationDlg();
		~CRotationDlg();

		RotationGroup GetRotationGroup();
		void SetRotationGroup(RotationGroup rg);

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_ROTATION_DLG };
#endif

	protected:
		void DoDataExchange(CDataExchange* pDX) override;

	protected:
		DECLARE_MESSAGE_MAP()

		BOOL OnInitDialog() override;
		void OnCancel() override;
		void OnOK() override;

		void InitializeSpeedCombo();
		void OnSpeedChanged();

		afx_msg void OnKillfocusXAngleEdit();
		afx_msg void OnKillfocusYAngleEdit();
		afx_msg void OnKillfocusZAngleEdit();

	private:
		RotationGroup m_rotationGroup;
		int m_action;
		float m_angleX;
		float m_angleY;
		float m_angleZ;
		int m_speed;
		bool m_rotateBackgroundWithMain;

		CComboBox m_SpeedCombo;
	};
}

