#include "stdafx.h"
#include "WorldScaleDlg.h"

#include "Resource.h"

using namespace DXF;
using namespace std;

namespace
{
	const float MIN_SCALE = 0.001f;
	const float MAX_SCALE = 10.0f;
}

class CWorldScaleDlgImp : public CWorldScaleDlg
{
private:
	Vertex<float> m_worldScale;
	Vertex<float> m_backgroundScale;

public:
	CWorldScaleDlgImp(const Vertex<float>& worldScale, const Vertex<float>& backgroundScale, CWnd* pParent)
		: CWorldScaleDlg(IDD_WORLD_SCALE_DLG, pParent)
		, m_worldScale(worldScale)
		, m_backgroundScale(backgroundScale)
	{}

	virtual ~CWorldScaleDlgImp() {}

	Vertex<float> GetWorldScale() const override
	{
		return m_worldScale;
	}

	DXF::Vertex<float> GetBackgroundScale() const override
	{
		return m_backgroundScale;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CWorldScaleDlg::OnInitDialog();

//		InitializeCtrls();

		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CWorldScaleDlg::DoDataExchange(pDX);

		DDX_Text(pDX, IDC_X_EDIT, m_worldScale.X);
		DDV_MinMaxFloat(pDX, m_worldScale.X, MIN_SCALE, MAX_SCALE);
		DDX_Text(pDX, IDC_Y_EDIT, m_worldScale.Y);
		DDV_MinMaxFloat(pDX, m_worldScale.Y, MIN_SCALE, MAX_SCALE);
		DDX_Text(pDX, IDC_Z_EDIT, m_worldScale.Z);
		DDV_MinMaxFloat(pDX, m_worldScale.Z, MIN_SCALE, MAX_SCALE);

		DDX_Text(pDX, IDC_BACKGROUND_X_EDIT, m_backgroundScale.X);
		DDV_MinMaxFloat(pDX, m_backgroundScale.X, MIN_SCALE, MAX_SCALE);
		DDX_Text(pDX, IDC_BACKGROUND_Y_EDIT, m_backgroundScale.Y);
		DDV_MinMaxFloat(pDX, m_backgroundScale.Y, MIN_SCALE, MAX_SCALE);
		DDX_Text(pDX, IDC_BACKGROUND_Z_EDIT, m_backgroundScale.Z);
		DDV_MinMaxFloat(pDX, m_backgroundScale.Z, MIN_SCALE, MAX_SCALE);
	}

	void OnOK() override
	{
		UpdateData(TRUE);

		CWorldScaleDlg::OnOK();
	}

};

BEGIN_MESSAGE_MAP(CWorldScaleDlgImp, CWorldScaleDlg)
END_MESSAGE_MAP()


shared_ptr<CWorldScaleDlg> CWorldScaleDlg::CreateWorldScaleDlg(const Vertex<float>& worldScale, 
	const Vertex<float>& backgroundScale, CWnd* pParent)
{
	return make_shared<CWorldScaleDlgImp>(worldScale, backgroundScale, pParent);
}