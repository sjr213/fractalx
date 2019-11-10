#pragma once
#include <Vertex.h>

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

	virtual void SetTarget(const DXF::Vertex<float>& target) = 0;

	virtual DXF::Vertex<float> GetTarget() const = 0;

	static std::shared_ptr<CPositionAngleDlg> CreatePositionAngleDlg(const DXF::RotationParams& rotationParams, 
		const DXF::Vertex<float>& target, CWnd* pParent = nullptr);

protected:
	CPositionAngleDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};