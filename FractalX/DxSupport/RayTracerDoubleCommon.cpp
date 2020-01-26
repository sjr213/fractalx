#include "stdafx.h"
#include "RayTracerDoubleCommon.h"

#include <algorithm>
#include <DirectXMath.h>
#include "RayTracerCommon.h"
#include "SphereApproximator.h"
#include "TraceParams.h"
#include "Vector3Double.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{

	double CalculateStepStretchDistance(const std::function<double(const Vector3Double&, const Vector3Double&, Vector3Double&)>& Marcher,
		const Vector3Double& start, const Vector3Double& direction, Vector3Double& p, const StretchDistanceParams& stretchParams)
	{
		double distance = Marcher(start, direction, p);

		if (distance < stretchParams.MinDistance)
			return 0.0;

		if (distance > stretchParams.MaxDistance)
			return 1.0;

		return (distance - stretchParams.MinDistance) / (stretchParams.MaxDistance - stretchParams.MinDistance);
	}


	StretchDistanceParams EstimateStepStretchRange(const std::function<double(const Vector3Double&, const Vector3Double&, Vector3Double&)>& Marcher,
		const TriangleData& data, const std::function<void(double)>& setProgress, const TraceParams& traceParams)
	{
		double minDistance = std::numeric_limits<double>::max();
		double maxDistance = 0.0;

		size_t nVertices = data.Vertices.size();

		for (size_t nVertex = 0; nVertex < nVertices; nVertex += 3)
		{
			const XMFLOAT3& v = data.Vertices.at(nVertex);
			Vector3Double pt = MakeStartingPoint(traceParams.Bulb.Distance, traceParams.Bulb.Origin, v, traceParams.Fractal.ConstantC);

			Vector3Double direction = -1.0f * Vector3Double(v);
			Vector3Double p;
			double distance = Marcher(pt, direction, p);

			minDistance = std::min(minDistance, distance);
			maxDistance = std::max(maxDistance, distance);

			if (nVertex % 30)
				setProgress(static_cast<double>(nVertex) / nVertices);
		}

		StretchDistanceParams stretchParams = traceParams.Stretch;
		stretchParams.MinDistance = minDistance;
		stretchParams.MaxDistance = maxDistance;

		return stretchParams;
	}
}