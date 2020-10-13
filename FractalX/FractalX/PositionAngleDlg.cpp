
#include "stdafx.h"
#include "PositionAngleDlg.h"

#include "ClickMappedPictureCtrl.h"
#include "FileSupport.h"
#include "Messages.h"
#include "Resource.h"
#include "RotationGroup.h"

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

	void LimitAngleToWithIn360(float& angle)
	{
		while (angle > 360.0f)
			angle -= 360.0f;

		while (angle < 0.0f)
			angle += 360.0f;
	}
}

class CPositionAngleDlgImp : public CPositionAngleDlg
{
private:
	CClickMappedPictureCtrl m_coordCtrl;
	RotationGroup m_rotationGroupOriginal;
	RotationGroup m_rotationGroup;
	Vertex<float> m_targetOriginal;
	Vertex<float> m_targetBackgroundOriginal;
	Vertex<float> m_target;
	Vertex<float> m_targetBackgnd;
	CWnd* m_parent = nullptr;
	float m_distance;
	float m_angle;
	int m_model;
	RotationParams m_rotationParams;
	Vertex<float> m_activeTarget;

public:
	CPositionAngleDlgImp(const DXF::RotationGroup& rotationGroup,
		const Vertex<float>& target, const DXF::Vertex<float>& targetBackgnd, CWnd* pParent)
		: CPositionAngleDlg(IDD, pParent)
		, m_rotationGroupOriginal(rotationGroup)
		, m_rotationGroup(rotationGroup)
		, m_targetOriginal(target)
		, m_targetBackgroundOriginal(targetBackgnd)
		, m_target(target)
		, m_targetBackgnd(targetBackgnd)
		, m_parent(pParent)
		, m_distance(0.02f)
		, m_angle(10.0f)
		, m_model(RotationSelectionTypeToInt(m_rotationGroup.RotationType))
		, m_rotationParams(m_rotationGroup.RotationType == RotationSelectionType::Background ? m_rotationGroup.RotationParamsBackground : m_rotationGroup.RotationParamsMain)
		, m_activeTarget(m_rotationGroup.RotationType == RotationSelectionType::Background ? m_targetBackgnd : m_target)
	{}

	virtual ~CPositionAngleDlgImp()
	{}

	void SetRotationGroup(const RotationGroup& rotationGroup) override
	{
		m_rotationGroup = rotationGroup;
	}

	std::shared_ptr<DXF::RotationGroup> GetRotationGroup() const override
	{
		return std::make_shared<RotationGroup>(m_rotationGroup);
	}

	void SetTarget(const Vertex<float>& target) override
	{
		m_target = target;
	}

	Vertex<float> GetTarget() const override
	{
		return m_target;
	}

	void SetTargetBackground(const DXF::Vertex<float>& targetBkgnd) override
	{
		m_targetBackgnd = targetBkgnd;
	}

	DXF::Vertex<float> GetTargetBackground() const override
	{
		return m_targetBackgnd;
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

		CString imagePath = FileSupport::GetExecutablePath() + _T("coords2.bmp");
		m_coordCtrl.SetBitmap(imagePath);
	}

	BOOL OnInitDialog() override
	{
		CPositionAngleDlg::OnInitDialog();

		PrepMappedBitmap();

		m_model = RotationSelectionTypeToInt(m_rotationGroup.RotationType);

		UpdateData(FALSE);

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CPositionAngleDlg::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_COORD_PIC, m_coordCtrl);

		DDX_Radio(pDX, IDC_MAIN_MODEL_RAD, m_model);

		DDX_Text(pDX, IDC_DISTANCE_EDIT, m_distance);
		DDV_MinMaxFloat(pDX, m_distance, 0.00001f, 10.0f);

		DDX_Text(pDX, IDC_ANGLE_EDIT, m_angle);
		DDV_MinMaxFloat(pDX, m_angle, 0.1f, 180.0f);

		DDX_Text(pDX, IDC_POS_X_EDIT, m_activeTarget.X);
		DDV_MinMaxFloat(pDX, m_activeTarget.X, -100.0f, +100.0f);

		DDX_Text(pDX, IDC_POS_Y_EDIT, m_activeTarget.Y);
		DDV_MinMaxFloat(pDX, m_activeTarget.Y, -100.0f, +100.0f);

		DDX_Text(pDX, IDC_POS_Z_EDIT, m_activeTarget.Z);
		DDV_MinMaxFloat(pDX, m_activeTarget.Z, -100.0f, +100.0f);

		float angleX = m_rotationParams.AngleXDegrees;
		DDX_Text(pDX, IDC_X_ANGLE_EDIT, angleX);
		DDV_MinMaxFloat(pDX, angleX, -360.0f, +360.0f);
		m_rotationParams.AngleXDegrees = angleX;

		float angleY = m_rotationParams.AngleYDegrees;
		DDX_Text(pDX, IDC_Y_ANGLE_EDIT, angleY);
		DDV_MinMaxFloat(pDX, angleY, -360.0f, +360.0f);
		m_rotationParams.AngleYDegrees = angleY;

		float angleZ = m_rotationParams.AngleZDegrees;
		DDX_Text(pDX, IDC_Z_ANGLE_EDIT, angleZ);
		DDV_MinMaxFloat(pDX, angleZ, -360.0f, +360.0f);
		m_rotationParams.AngleZDegrees = angleZ;

		SaveModel();
	}

	void SaveModel()
	{
		auto currentModel = RotationSelectionTypeFromInt(m_model);
		if (m_rotationGroup.RotationType == RotationSelectionType::Background)
		{
			m_rotationGroup.RotationParamsBackground = m_rotationParams;
			m_targetBackgnd = m_activeTarget;
		}
		else
		{
			m_rotationGroup.RotationParamsMain = m_rotationParams;
			m_target = m_activeTarget;
		}

		if (m_rotationGroup.RotationType != currentModel)
		{
			m_rotationGroup.RotationType = currentModel;
			if (m_rotationGroup.RotationType == RotationSelectionType::Background)
			{
				m_rotationParams = m_rotationGroup.RotationParamsBackground;
				m_activeTarget = m_targetBackgnd;
			}
			else
			{
				m_rotationParams = m_rotationGroup.RotationParamsMain;
				m_activeTarget = m_target;
			}
		}	
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

		m_rotationGroup = m_rotationGroupOriginal;

		m_target = m_targetOriginal;

		m_targetBackgnd = m_targetBackgroundOriginal;

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

		UpdateData(TRUE);

		if (!m_parent)
		{
			ASSERT(false);
			return 0;
		}

		switch (static_cast<int>(wparam))
		{
		case X_Minus:
			m_activeTarget.X = m_activeTarget.X - m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case X_CCW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleXDegrees += m_angle;
				LimitAngleToWithIn360(m_rotationParams.AngleXDegrees);
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case X_Plus:
			m_activeTarget.X = m_activeTarget.X + m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case X_CW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleXDegrees -= m_angle;
				LimitAngleToWithIn360(m_rotationParams.AngleXDegrees);
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Y_Minus:
			m_activeTarget.Y = m_activeTarget.Y - m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Y_CCW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleYDegrees += m_angle;
				LimitAngleToWithIn360(m_rotationParams.AngleYDegrees);
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Y_Plus:
			m_activeTarget.Y = m_activeTarget.Y + m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Y_CW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleYDegrees -= m_angle;
				LimitAngleToWithIn360(m_rotationParams.AngleYDegrees);
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Z_Minus:
			m_activeTarget.Z = m_activeTarget.Z - m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Z_CCW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleZDegrees += m_angle;
				LimitAngleToWithIn360(m_rotationParams.AngleZDegrees);
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		case Z_Plus:
			m_activeTarget.Z = m_activeTarget.Z + m_distance;
			m_parent->PostMessage(cMessage::tm_modelPositionChanged);
			break;
		case Z_CW:
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_rotationParams.AngleZDegrees -= m_angle;
				LimitAngleToWithIn360(m_rotationParams.AngleZDegrees);
				m_parent->PostMessage(cMessage::tm_modelAngleChanged);
			}
			break;
		}

		UpdateData(FALSE);
		return 0;
	}

	void OnKillFocus()
	{
		UpdateData(TRUE);
	}

	void OnPositionChanged()
	{
		UpdateData(TRUE);
		m_parent->PostMessage(cMessage::tm_modelPositionChanged);
	}

	void OnAngleChanged()
	{
		UpdateData(TRUE);
		m_parent->PostMessage(cMessage::tm_modelAngleChanged);
	}

	afx_msg void OnBnModelClicked()
	{
		UpdateData(TRUE);
		UpdateData(FALSE);

		m_parent->PostMessage(cMessage::tm_modelAngleChanged);
	}
	
};

BEGIN_MESSAGE_MAP(CPositionAngleDlgImp, CPositionAngleDlg)
	ON_BN_CLICKED(IDOK, &CPositionAngleDlgImp::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CPositionAngleDlgImp::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_MAIN_MODEL_RAD, &CPositionAngleDlgImp::OnBnModelClicked)
	ON_BN_CLICKED(IDC_BACKGROUND_MODEL_RAD, &CPositionAngleDlgImp::OnBnModelClicked)
	ON_BN_CLICKED(IDC_COPY_MAIN_TO_BKGND_RAD, &CPositionAngleDlgImp::OnBnModelClicked)
	ON_REGISTERED_MESSAGE(cMessage::tm_clickIDs, &CPositionAngleDlgImp::OnTargetClicked)
	ON_EN_KILLFOCUS(IDC_DISTANCE_EDIT, &CPositionAngleDlgImp::OnKillFocus)
	ON_EN_KILLFOCUS(IDC_ANGLE_EDIT, &CPositionAngleDlgImp::OnKillFocus)
	ON_EN_KILLFOCUS(IDC_POS_X_EDIT, &CPositionAngleDlgImp::OnPositionChanged)
	ON_EN_KILLFOCUS(IDC_POS_Y_EDIT, &CPositionAngleDlgImp::OnPositionChanged)
	ON_EN_KILLFOCUS(IDC_POS_Z_EDIT, &CPositionAngleDlgImp::OnPositionChanged)
	ON_EN_KILLFOCUS(IDC_X_ANGLE_EDIT, &CPositionAngleDlgImp::OnAngleChanged)
	ON_EN_KILLFOCUS(IDC_Y_ANGLE_EDIT, &CPositionAngleDlgImp::OnAngleChanged)
	ON_EN_KILLFOCUS(IDC_Z_ANGLE_EDIT, &CPositionAngleDlgImp::OnAngleChanged)
END_MESSAGE_MAP()

std::shared_ptr<CPositionAngleDlg> CPositionAngleDlg::CreatePositionAngleDlg(const RotationGroup& rotationGroup,
	const Vertex<float>& target, const DXF::Vertex<float>& targetBackgnd, CWnd* pParent)
{
	return std::make_shared <CPositionAngleDlgImp>(rotationGroup, target, targetBackgnd, pParent);
}