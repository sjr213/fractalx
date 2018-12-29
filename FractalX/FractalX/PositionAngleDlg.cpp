
#include "stdafx.h"
#include "PositionAngleDlg.h"

#include "Messages.h"
#include "Resource.h"
#include <RotationParams.h>

using namespace DXF;

class CPositionAngleDlgImp : public CPositionAngleDlg
{
private:
	RotationParams m_rotationParamsOriginal;
	RotationParams m_rotationParams;
	std::tuple<float, float, float> m_targetOriginal;
	std::tuple<float, float, float> m_target;
	CWnd* m_parent = nullptr;

public:
	CPositionAngleDlgImp(const DXF::RotationParams& rotationParams,
		const std::tuple<float, float, float>& target, CWnd* pParent)
		: CPositionAngleDlg(IDD, pParent)
		, m_rotationParamsOriginal(rotationParams)
		, m_rotationParams(rotationParams)
		, m_targetOriginal(target)
		, m_target(target)
		, m_parent(pParent)
	{}

	virtual ~CPositionAngleDlgImp()
	{
	}

	void SetRotationParams(const RotationParams& rotationParams) override
	{
		m_rotationParams = rotationParams;
	}

	std::shared_ptr<DXF::RotationParams> GetRotationParams() const override
	{
		return std::make_shared<RotationParams>(m_rotationParams);
	}

	void SetTarget(const std::tuple<float, float, float>& target) override
	{
		m_target = target;
	}

	std::tuple<float, float, float> GetTarget() const override
	{
		return m_target;
	}

	// Dialog Data
	enum { IDD = IDD_POSITION_ANGLE_DLG };

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CPositionAngleDlg::OnInitDialog();

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPositionAngleDlg::DoDataExchange(pDX);
	}

	afx_msg void OnOk()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		m_parent->PostMessage(cMessage::tm_killPositionAngleDlg, cMessage::DlgOK, 0);
	}

	afx_msg void OnBnClickedCancel()
	{
		if (!m_parent)
		{
			ASSERT(false);
			return;
		}

		m_rotationParams = m_rotationParamsOriginal;

		m_target = m_targetOriginal;

		m_parent->PostMessage(cMessage::tm_killPositionAngleDlg, cMessage::DlgCancel, 0);
	}
};

BEGIN_MESSAGE_MAP(CPositionAngleDlgImp, CPositionAngleDlg)
	ON_BN_CLICKED(IDOK, &CPositionAngleDlgImp::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CPositionAngleDlgImp::OnBnClickedCancel)
//	ON_EN_KILLFOCUS(IDC_PALETTE_NAME_EDIT, &CPaletteViewDlgImp::OnKillfocusEdit)
//	ON_REGISTERED_MESSAGE(cMessage::tm_pinsChanged, &CPaletteViewDlgImp::OnPinsChanged)
END_MESSAGE_MAP()

std::shared_ptr<CPositionAngleDlg> CPositionAngleDlg::CreatePositionAngleDlg(const RotationParams& rotationParams,
	const std::tuple<float, float, float>& target, CWnd* pParent)
{
	return std::make_shared <CPositionAngleDlgImp>(rotationParams, target, pParent);
}