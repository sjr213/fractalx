#include "stdafx.h"
#include "InglesRayTracer2.h"

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
	class InglesRayTracer2 : public IRayTracer
	{
	private:
		TraceParams m_traceParams;

	protected:

		void CalculateNextCycle(const Vector3Double& z, const Vector3Double& dz, Vector3Double& z1, Vector3Double& dz1)
		{
			z1.x = z.x * z.x - z.y * z.y - z.z * z.z + m_traceParams.Fractal.ConstantC.X;
			z1.y = 2.0 * z.x * z.y + m_traceParams.Fractal.ConstantC.Y;
			z1.z = 2.0 * z.x * z.z + m_traceParams.Fractal.ConstantC.Z;

			dz1.x = 2.0 * z.x * dz.x - z.y * dz.y - z.z * dz.z;
			dz1.y = z.y * dz.x + z.x + dz.y;
			dz1.z = z.x * dz.z + z.z * dz.x;
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

		InglesRayTracer2(const TraceParams& traceParams)
			: m_traceParams(traceParams)
		{}

		virtual ~InglesRayTracer2()
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

	std::unique_ptr<IRayTracer> CreateInglesRayTracer2(const TraceParams& traceParams)
	{
		return std::make_unique<InglesRayTracer2>(traceParams);
	}
}