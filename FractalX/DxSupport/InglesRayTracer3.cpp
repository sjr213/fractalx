#include "stdafx.h"
#include "InglesRayTracer3.h"

#include <algorithm>
#include <limits>
#include "RayTracerCommon.h"
#include "RayTracerDoubleCommon.h"
#include <SimpleMath.h>
#include "SphereApproximator.h"
#include "TraceParams.h"
#include "VertexData.h"
#include "Vector3Double.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	class InglesRayTracer3 : public IRayTracer
	{
	private:
		TraceParams m_traceParams;

	protected:

		void CalculateNextCycle(const Vector3Double& Q, const Vector3Double& dQ, Vector3Double& Q1, Vector3Double& dQ1)
		{
			Q1.x = Q.x * Q.x - Q.y * Q.y - Q.z * Q.z + m_traceParams.Fractal.ConstantC.X;
			Q1.y = 2.0 * Q.x * Q.y + m_traceParams.Fractal.ConstantC.Y;
			Q1.z = 2.0 * Q.x * Q.z + m_traceParams.Fractal.ConstantC.Z;

			dQ1.x = 2.0 * Q.x * dQ.x - 2.0 * Q.y * dQ.y - 2.0 * Q.z * dQ.z;
			dQ1.y = 2.0 * Q.y * dQ.x + 2.0 * Q.x + dQ.y;
			dQ1.z = 2.0 * Q.z * dQ.x + 2.0 * Q.x * dQ.z;
		}

		double EstimateDistance(Vector3Double& pos, const TraceParams& traceParams)
		{
			Vector3Double z = pos;
			Vector3Double dz(1.0, 1.0, 1.0);
			double dr = 1.0;
			double r = 0.0;
			Vector3Double z1;
			Vector3Double dz1;

			if (traceParams.Fractal.NormalizationType == BulbNormalizeType::AltRoots)
			{
				r = z.Length(traceParams.Fractal.NormalizationRoot);
			}
			else
			{
				r = z.Length();
			}

			int i = 0;
			for (; i < traceParams.Bulb.Iterations; ++i)
			{
				if (r > traceParams.Fractal.Bailout)
					break;

				CalculateNextCycle(z, dz, z1, dz1);

				if (IsNan(z1))
					break;

				if (traceParams.Fractal.NormalizationType == BulbNormalizeType::AltRoots)
				{
					r = z1.Length(traceParams.Fractal.NormalizationRoot);
					dr = dz1.Length(traceParams.Fractal.NormalizationRoot);
				}
				else
				{
					r = z1.Length();
					dr = dz1.Length();
				}

				z = z1;
				dz = dz1;

				z += pos;
			}

			if (r == 0.0 || dr == 0.0)
				return 0.0;

			//return 0.5 * log(r) * r / dr;
			// see https://www.shadertoy.com/view/MsfGRr
			return 0.25 * sqrt(r / dr) * exp2(-1 * i) * log(r);
		}

	public:

		InglesRayTracer3(const TraceParams& traceParams)
			: m_traceParams(traceParams)
		{}

		virtual ~InglesRayTracer3()
		{}

		std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const std::function<void(double)>& setProgress) override
		{
			auto tracer = CreateInternalDoubleRayTracer(m_traceParams, [this](Vector3Double& pos, const TraceParams& traceParams)
				{
					return EstimateDistance(pos, traceParams);
				});

			return tracer->RayTrace(data, setProgress);
		}

		std::shared_ptr<DxVertexData> RayTraceStretch(const TriangleData& data, const std::function<void(double)>& setProgress) override
		{
			auto tracer = CreateInternalDoubleRayTracer(m_traceParams, [this](Vector3Double& pos, const TraceParams& traceParams)
				{
					return EstimateDistance(pos, traceParams);
				});

			return tracer->RayTraceStretch(data, setProgress);
		}

	};

	std::unique_ptr<IRayTracer> CreateInglesRayTracer3(const TraceParams& traceParams)
	{
		return std::make_unique<InglesRayTracer3>(traceParams);
	}
}