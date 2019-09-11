#pragma once

namespace DXF
{
	struct DxVertexData;
	struct TraceParams;
	struct TriangleData;

	struct IRayTracer
	{
		virtual ~IRayTracer() {}

		virtual std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const std::function<void(double)>& setProgress) = 0;

		virtual std::shared_ptr<DxVertexData> RayTraceStretch(const TriangleData& data, const std::function<void(double)>& setProgress) = 0;
	};


}