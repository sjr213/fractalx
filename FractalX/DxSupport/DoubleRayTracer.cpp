#include "stdafx.h"
#include "DoubleRayTracer.h"

#include <algorithm>
#include "CartesianConverterFactory.h"
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
	class DoubleRayTracer : public IRayTracer
	{
	private:
		TraceParams m_traceParams;
		std::function<void(Vector3Double&, double, double, double)> m_cartesianConverter;

	protected:

		// converted
		void CalculateNextCycle(Vector3Double& z, double& r, double& dr)
		{
			// convert to polar coordinates
			double theta = acos(z.z / r);
			double phi = atan2(z.y, z.x);

			dr = pow(r, m_traceParams.Fractal.Power - 1.0) * m_traceParams.Fractal.Power * dr + m_traceParams.Fractal.Derivative;

			// scale and rotate the point
			double zr = pow(r, m_traceParams.Fractal.Power);
			theta = theta * m_traceParams.Fractal.Power;
			phi = phi * m_traceParams.Fractal.Power;

			// convert back to Cartesian coordinates
			m_cartesianConverter(z, zr, theta, phi);
		}

		double EstimateDistance(Vector3Double& pos, const TraceParams& traceParams)
		{
			Vector3Double z = pos + m_traceParams.Fractal.ConstantC;
			double dr = 1.0;
			double r = 0.0;

			for (int i = 0; i < traceParams.Bulb.Iterations; ++i)
			{
				if (traceParams.Fractal.NormalizationType == BulbNormalizeType::AltRoots)
					r = z.Length(traceParams.Fractal.NormalizationRoot);
				else
					r = z.Length();

				if (r > traceParams.Fractal.Bailout)
					break;

				CalculateNextCycle(z, r, dr);

				if (IsNan(z))
					break;

				z += pos;
			}

			if (r == 0.0 || dr == 0.0)
				return 0.0;

			return 0.5 * log(r) * r / dr;
		}

	public:

		DoubleRayTracer(const TraceParams& traceParams,
			const std::function<void(Vector3Double&, const double&, const double&, const double&)>& cartesianConverter)
			: m_traceParams(traceParams)
			, m_cartesianConverter(cartesianConverter)
		{}

		virtual ~DoubleRayTracer()
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

	std::unique_ptr<IRayTracer> CreateDoubleRayTracer(const TraceParams& traceParams)
	{
		auto converter = CreateConverter(traceParams.Fractal.ConversionGroup);
		return std::make_unique<DoubleRayTracer>(traceParams, converter);
	}
}