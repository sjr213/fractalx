
#include "stdafx.h"
#include "PositionAngleDlg.h"

#include "ClickMappedPictureCtrl.h"
#include "Messages.h"
#include "Resource.h"
#include "RotationParams.h"

using namespace DXF;

namespace
{
	constexpr int X_Minus = 10;
	constexpr int X_CCW = 11;
	constexpr int X_Plus = 12;
	constexpr int X_CW = 13;

	constexpr int Y_Minus = 20;
	constexpr int Y_CCW = 21;
	constexpr int Y_Plus = 22;
	constexpr int Y_CW = 23;

	constexpr int Z_Minus = 30;
	constexpr int Z_CCW = 31;
	constexpr int Z_Plus = 32;
	constexpr int Z_CW = 33;
}

class CPositionAngleDlgImp : public CPositionAngleDlg
{
private:
	CClickMappedPictureCtrl m_coordCtrl;
	RotationParams m_rotationParamsOriginal;
	RotationParams m_rotationParams;
	std::tuple<float, float, float> m_targetOriginal;
	std::tuple<float, float, float> m_target;
	CWnd* m_parent = nullptr;
	float m_distance;
	float m_angle;

public:
	CPositionAngleDlgImp(const DXF::RotationParams& rotationParams,
		const std::tuple<float, float, float>& target, CWnd* pParent)
		: CPositionAngleDlg(IDD, pParent)
		, m_rotationParamsOriginal(rotationParams)
		, m_rotationParams(rotationParams)
		, m_targetOriginal(target)
		, m_target(target)
		, m_parent(pParent)
		, m_distance(0.02f)
		, m_angle(10.0f)
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

	std::list<PictureClickTarget> BuildClickTargets()
	{
		std::list<PictureClickTarget> targets
		{
			{CRect(10, 150, 85, 210), CRect(10, 150, 43, 210), X_Minus },
			{CRect(315, 150, 348, 210), CRect(315, 150, 348, 210), X_CCW},
			{CRect(350, 150, 382, 210), CRect(350, 150, 382, 210), X_Plus},
			{CRect(275, 150, 310, 210), CRect(350, 150, 382, 210), X_Plus},
			{CRect(384, 150, 417, 210), CRect(384, 150, 417, 210), X_CW},
			{CRect(159, 277, 208, 350), CRect(159, 317, 208, 350), Y_Minus},
			{CRect(132, 5, 167, 62), CRect(132, 5, 167, 62), Y_CCW},
			{CRect(169, 5, 201, 87), CRect(169, 5, 201, 38), Y_Plus},
			{CRect(203, 5, 238, 62), CRect(203, 5, 238, 62), Y_CW},
			{CRect(247, 99, 320, 148), CRect(280,99, 320, 148), Z_Minus},
			{CRect(16,234,56,282), CRect(16,234,56,282), Z_CCW},
			{CRect(57,234,84,282), CRect(57,234,84,282), Z_Plus},
			{CRect(96,200,135,233), CRect(57,234,84,282), Z_Plus},
			{CRect(85,234,116, 282), CRect(85,234,116, 282), Z_CW}
		};

		return targets;
	}

	void PrepMappedBitmap()
	{
		m_coordCtrl.SetParent(this);
		m_coordCtrl.SendMousePositionMessage(false);
		m_coordCtrl.SendClickMessage(true);

		auto targets = BuildClickTargets();
		m_coordCtrl.SetClickTargets(targets);

		// Change this later to something more reproducible
		CString imagePath(_T("C:\\docs\\programming FractalX\\images\\coords2.png"));
		m_coordCtrl.SetBitmap(imagePath);
	}

	BOOL OnInitDialog() override
	{
		CPositionAngleDlg::OnInitDialog();

		PrepMappedBitmap();

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPositionAngleDlg::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_COORD_PIC, m_coordCtrl);

		DDX_Text(pDX, IDC_DISTANCE_EDIT, m_distance);
		DDV_MinMaxFloat(pDX, m_distance, 0.00001f, 10.0f);

		DDX_Text(pDX, IDC_ANGLE_EDIT, m_angle);
		DDV_MinMaxFloat(pDX, m_angle, 0.1f, 180.0f);
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

	afx_msg LRESULT OnMouseMoveMsg(WPARAM x, LPARAM y)
	{
		m_distance = static_cast<float>(x);
		m_angle = static_cast<float>(y);

		UpdateData(FALSE);

		return 0;
	}

	afx_msg LRESULT OnTargetClicked(WPARAM wparam, LPARAM)
	{
		ASSERT(wparam > 0);

		if (!m_parent)
		{
			ASSERT(false);
			return 0;
		}

		switch (static_cast<int>(wparam))
		{
		case X_Minus:
			std::get<0>(m_target) = std::get<0>(m_target) - m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case X_CCW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleXDegrees += m_angle;
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case X_Plus:
			std::get<0>(m_target) = std::get<0>(m_target) + m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case X_CW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleXDegrees -= m_angle;
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Y_Minus:
			std::get<1>(m_target) = std::get<1>(m_target) - m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Y_CCW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleYDegrees += m_angle;
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Y_Plus:
			std::get<1>(m_target) = std::get<1>(m_target) + m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Y_CW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleYDegrees -= m_angle;
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Z_Minus:
			std::get<2>(m_target) = std::get<2>(m_target) - m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Z_CCW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleZDegrees += m_angle;
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Z_Plus:
			std::get<2>(m_target) = std::get<2>(m_target) + m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Z_CW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleZDegrees -= m_angle;
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		}

		return 0;
	}

	void OnKillFocus()
	{
		UpdateData(TRUE);
	}
	
};

BEGIN_MESSAGE_MAP(CPositionAngleDlgImp, CPositionAngleDlg)
	ON_BN_CLICKED(IDOK, &CPositionAngleDlgImp::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CPositionAngleDlgImp::OnBnClickedCancel)
//	ON_REGISTERED_MESSAGE(cMessage::tm_mouseCoords, &CPositionAngleDlgImp::OnMouseMoveMsg)
	ON_REGISTERED_MESSAGE(cMessage::tm_clickIDs, &CPositionAngleDlgImp::OnTargetClicked)
	ON_EN_KILLFOCUS(IDC_DISTANCE_EDIT, &CPositionAngleDlgImp::OnKillFocus)
	ON_EN_KILLFOCUS(IDC_ANGLE_EDIT, &CPositionAngleDlgImp::OnKillFocus)
//	ON_REGISTERED_MESSAGE(cMessage::tm_pinsChanged, &CPaletteViewDlgImp::OnPinsChanged)
END_MESSAGE_MAP()

std::shared_ptr<CPositionAngleDlg> CPositionAngleDlg::CreatePositionAngleDlg(const RotationParams& rotationParams,
	const std::tuple<float, float, float>& target, CWnd* pParent)
{
	return std::make_shared <CPositionAngleDlgImp>(rotationParams, target, pParent);
}