#pragma once

namespace DXF
{
	struct DxVertexData;
	struct TraceParams;
	struct TriangleData;

	class BasicRayTracer
	{
	public:
		static std::shared_ptr<BasicRayTracer> CreateBasicRayTracer();

		virtual std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const TraceParams& traceParams, 
			const std::function<void(double)>& setProgress) = 0;

		virtual std::shared_ptr<DxVertexData> RayTraceStretch(const TriangleData& data, const TraceParams& traceParams,
			const std::function<void(double)>& setProgress) = 0;
	};

	
}