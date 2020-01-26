#pragma once

#include <functional>


namespace DXF
{
	struct StretchDistanceParams;
	struct TraceParams;
	struct TriangleData;
	struct Vector3Double;

	double CalculateStepStretchDistance(const std::function<double(const Vector3Double&, const Vector3Double&, Vector3Double&)>& Marcher,
		const Vector3Double& start, const Vector3Double& direction, Vector3Double& p, const StretchDistanceParams& stretchParams);

	StretchDistanceParams EstimateStepStretchRange(const std::function<double(const Vector3Double&, const Vector3Double&, Vector3Double&)>& Marcher,
		const TriangleData& data, const std::function<void(double)>& setProgress, const TraceParams& traceParams);
}