#pragma once

#include "RotationGroup.h"
#include "Vertex.h"

namespace DxSupport::Dx12CoordinateMap
{
	DXF::Vertex<float> ToD12Target(const DXF::Vertex<float>& target);

	DXF::Vertex<float> FromD12Target(const DXF::Vertex<float>& target);

	DXF::RotationParams ToD12RotationParams(const DXF::RotationParams& rt);

	DXF::RotationParams FromD12RotationParams(const DXF::RotationParams& rt);

	DXF::RotationGroup ToD12RotationGroup(const DXF::RotationGroup& rg);

	DXF::RotationGroup FromD12RotationGroup(const DXF::RotationGroup& rg);
}
