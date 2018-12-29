#pragma once

namespace DXF
{
	struct RotationParams;
}

class CPositionAngleDlg : public CDialogEx
{
public:
	virtual ~CPositionAngleDlg() {};

	virtual void SetRotationParams(const DXF::RotationParams& rotationParams) = 0;

	virtual std::shared_ptr<DXF::RotationParams> GetRotationParams() const = 0;

	virtual void SetTarget(const std::tuple<float, float, float>& target) = 0;

	virtual std::tuple<float, float, float> GetTarget() const = 0;

	static std::shared_ptr<CPositionAngleDlg> CreatePositionAngleDlg(const DXF::RotationParams& rotationParams, 
		const std::tuple<float, float, float>& target, CWnd* pParent = nullptr);

protected:
	CPositionAngleDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};