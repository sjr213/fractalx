
#include "stdafx.h"
#include "BasicRayTracer.h"

#include <algorithm>
#include <limits>
#include "RayTracerCommon.h"
#include <SimpleMath.h>
#include "SphereApproximator.h"
#include "TraceParams.h"
#include "VertexData.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	namespace
	{
		bool IsNan(Vector3 p)
		{
			if (isnan(p.x) || isnan(p.y) || isnan(p.z))
				return true;

			return false;
		}

		// Cartesian converters
		void StandardCartesianConverter(XMFLOAT3& z, double zr, double theta, double phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(sin(theta) * cos(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(cos(theta)));
		}

		void CartesianConverterAltX1(XMFLOAT3& z, double zr, double theta, double phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(sin(theta) * tan(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(cos(theta)));
		}

		void CartesianConverterAltX2(XMFLOAT3& z, double zr, double theta, double phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(cos(theta) * cos(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(cos(theta)));
		}

		void CartesianConverterAltY1(XMFLOAT3& z, double zr, double theta, double phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(sin(theta) * cos(phi)), static_cast<float>(sin(phi) * tan(theta)), static_cast<float>(cos(theta)));
		}

		void CartesianConverterAltZ1(XMFLOAT3& z, double zr, double theta, double phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(sin(theta) * cos(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(sin(theta)*cos(theta)));
		}
	}

	class BasicRayTracer : public IRayTracer
	{
	private:
		TraceParams m_traceParams;
		std::function<void(XMFLOAT3&, double, double, double)> m_cartesianConverter;

	protected:

		// This may vary
		void CalculateNextCycle(XMFLOAT3& z, double& r, double& dr)
		{
			// convert to polar coordinates
			double theta = acos(z.z / r);
			double phi = atan2(z.y, z.x);

			dr = pow(r, m_traceParams.Fractal.Power - 1.0) *  m_traceParams.Fractal.Power * dr + m_traceParams.Fractal.ConstantC;

			// scale and rotate the point
			double zr = pow(r, m_traceParams.Fractal.Power);
			theta = theta * m_traceParams.Fractal.Power;
			phi = phi * m_traceParams.Fractal.Power;

			// convert back to Cartesian coordinates
			m_cartesianConverter(z, zr, theta, phi);
		}

		double Normalize(Vector3 p)
		{
			if (m_traceParams.Fractal.NormalizationType == BulbNormalizeType::AltRoots)
			{
//				double sum = pow(p.x, m_traceParams.Fractal.NormalizationRoot) + 
//					pow(p.y, m_traceParams.Fractal.NormalizationRoot) + 
//					pow(p.z, m_traceParams.Fractal.NormalizationRoot);

				double x = p.x;
				double y = p.y;
				double z = p.z;

				double sum = x*x + y*y + z*z;

				return pow(sum, m_traceParams.Fractal.NormalizationRoot);
			}
			else
				return p.Length();
		}

		double EstimateDistance(XMFLOAT3& pos)
		{
			Vector3 z = pos;
			double dr = 1.0;
			double r = 0.0;

			for (int i = 0; i < m_traceParams.Bulb.Iterations; ++i)
			{
				r = Normalize(z);
				if (r > m_traceParams.Fractal.Bailout)
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


		double RayMarch(const XMFLOAT3& start, const XMFLOAT3& direction, XMFLOAT3& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; steps++)
			{
				p = static_cast<float>(totalDistance) * direction + start;
				double distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.Bulb.StepDivisor;                      // note change 
				if (distance < m_traceParams.Bulb.MinRayDistance || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			return 1.0 - ((double)steps) / m_traceParams.Bulb.MaxRaySteps;
		}

		double RayMarchFractional(const XMFLOAT3& start, const XMFLOAT3& direction, XMFLOAT3& p)
		{
			double totalDistance = 0.0;
			double distance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; ++steps)
			{
				p = static_cast<float>(totalDistance) * direction + start;
				distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.Bulb.StepDivisor;                     
				if (distance < m_traceParams.Bulb.MinRayDistance) // || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			double delta = steps;
			if (steps < m_traceParams.Bulb.MaxRaySteps - 1 && distance < m_traceParams.Bulb.MinRayDistance)
				delta = steps + distance / m_traceParams.Bulb.MinRayDistance;

			delta = std::min(delta, static_cast<double>(m_traceParams.Bulb.MaxRaySteps));

			return 1.0 - delta / m_traceParams.Bulb.MaxRaySteps;
		}

		double RayMarchDistance(const XMFLOAT3& start, const XMFLOAT3& direction, XMFLOAT3& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; steps++)
			{
				p = static_cast<float>(totalDistance) * direction + start;
				double distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.Bulb.StepDivisor;                      // note change 
				if (distance < m_traceParams.Bulb.MinRayDistance || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			// We might want to set lastDistance to distance if it breaks on first step
			return lastDistance;
		}

		StretchDistanceParams EstimateStretchRange(const TriangleData& data, const std::function<void(double)>& setProgress)
		{
			double minDistance = std::numeric_limits<double>::max();
			double maxDistance = 0.0;

			size_t nVertices = data.Vertices.size();

			for (size_t nVertex = 0; nVertex < nVertices; nVertex += 3)
			{
				const XMFLOAT3& v = data.Vertices.at(nVertex);
				XMFLOAT3 pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v);

				XMFLOAT3 direction = -1.0f * v;
				XMFLOAT3 p;
				double distance = RayMarchDistance(pt, direction, p);

				minDistance = std::min(minDistance, distance);
				maxDistance = std::max(maxDistance, distance);

				if (nVertex % 30)
					setProgress(static_cast<double>(nVertex) / nVertices);
			}

			StretchDistanceParams stretchParams = m_traceParams.Stretch;
			stretchParams.MinDistance = minDistance;
			stretchParams.MaxDistance = maxDistance;

			return stretchParams;
		}

		double CalculateStretchDistance(const XMFLOAT3& start, const XMFLOAT3& direction, XMFLOAT3& p, const StretchDistanceParams& stretchParams)
		{
			double distance = RayMarchDistance(start, direction, p);

			if (distance < stretchParams.MinDistance)
				return 0.0;

			if (distance > stretchParams.MaxDistance)
				return 1.0;

			return (distance - stretchParams.MinDistance) / (stretchParams.MaxDistance - stretchParams.MinDistance);
		}

	public:

		BasicRayTracer(const TraceParams& traceParams,
			const std::function<void(XMFLOAT3&, const double&, const double&, const double&)>& cartesianConverter)
			: m_traceParams(traceParams)
			, m_cartesianConverter(cartesianConverter)
		{}

		virtual ~BasicRayTracer()
		{}

		std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const std::function<void(double)>& setProgress) override
		{
			std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();
			vData->StretchParams = m_traceParams.Stretch;

			double total = data.Vertices.size() + 2.0;
			int progress = 1;

			std::function<double(const XMFLOAT3&, const XMFLOAT3&, XMFLOAT3&)> marcher =
				[this](const XMFLOAT3& start, const XMFLOAT3& direction, XMFLOAT3& p)
			{
				return RayMarch(start, direction, p);
			};
			if (m_traceParams.Bulb.Fractional)
				marcher = [this](const XMFLOAT3& start, const XMFLOAT3& direction, XMFLOAT3& p)
			{
				return RayMarchFractional(start, direction, p);
			};

			XMFLOAT3 nullNormal(0.0f, 0.0f, 0.0f);

			for (const XMFLOAT3& v : data.Vertices)
			{
				XMFLOAT3 pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v);

				XMFLOAT3 direction = -1.0f * v;
				Vector3 p;
				double distance = marcher(pt, direction, p);

				vData->Vertices.emplace_back(p, nullNormal, Vector2(static_cast<float>(distance), 0.0f));

				++progress;
				if (progress % 20 == 0)
					setProgress(progress / total);
			}

			for (const Triangle& t : data.Triangles)
			{
				vData->Indices.push_back(t.one);
				vData->Indices.push_back(t.two);
				vData->Indices.push_back(t.three);
			}

			CalculateNormals(*vData);

			setProgress(1.0);

			return vData;
		}

		std::shared_ptr<DxVertexData> RayTraceStretch(const TriangleData& data, const std::function<void(double)>& setProgress) override
		{
			auto setLocalProgress = setProgress;

			if (m_traceParams.Stretch.EstimateMinMax)
			{
				setLocalProgress = [&](double progress)
				{
					setProgress(0.25 + 3.0*progress / 4.0);
				};

				m_traceParams.Stretch = EstimateStretchRange(data, [&](double progress)
				{
					setProgress(progress / 4.0);
				});
			}
				
			std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();
			vData->StretchParams = m_traceParams.Stretch;

			double total = data.Vertices.size() + 2.0;
			int progress = 1;

			XMFLOAT3 nullNormal(0.0f, 0.0f, 0.0f);

			for (const XMFLOAT3& v : data.Vertices)
			{
				XMFLOAT3 pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v);

				XMFLOAT3 direction = -1.0f * v;
				XMFLOAT3 p;
				double distance = CalculateStretchDistance(pt, direction, p, m_traceParams.Stretch);

				vData->Vertices.emplace_back(p, nullNormal, Vector2(static_cast<float>(distance), 0.0f));

				++progress;
				if (progress % 20 == 0)
					setLocalProgress(progress / total);
			}

			for (const Triangle& t : data.Triangles)
			{
				vData->Indices.push_back(t.one);
				vData->Indices.push_back(t.two);
				vData->Indices.push_back(t.three);
			}

			CalculateNormals(*vData);

			setProgress(1.0);

			return vData;
		}

	};

	std::unique_ptr<IRayTracer> CreateBasicRayTracer(const TraceParams& traceParams)
	{
		if(traceParams.Fractal.CartesianType == CartesianConversionType::CartesianConvertAltX1)
			return std::make_unique<BasicRayTracer>(traceParams, CartesianConverterAltX1);

		if (traceParams.Fractal.CartesianType == CartesianConversionType::CartesianConvertAltX2)
			return std::make_unique<BasicRayTracer>(traceParams, CartesianConverterAltX2);

		if (traceParams.Fractal.CartesianType == CartesianConversionType::CartesianConvertAltY1)
			return std::make_unique<BasicRayTracer>(traceParams, CartesianConverterAltY1);

		if (traceParams.Fractal.CartesianType == CartesianConversionType::CartesianConvertAltZ1)
			return std::make_unique<BasicRayTracer>(traceParams, CartesianConverterAltZ1);

		return std::make_unique<BasicRayTracer>(traceParams, StandardCartesianConverter);
	}
}