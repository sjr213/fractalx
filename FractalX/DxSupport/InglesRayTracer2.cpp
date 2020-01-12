#include "stdafx.h"
#include "InglesRayTracer2.h"

#include <algorithm>
#include <limits>
#include "RayTracerCommon.h"
#include <SimpleMath.h>
#include "SphereApproximator.h"
#include "TraceParams.h"
#include "VertexData.h"
#include "Vector3Double.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	namespace
	{
		bool IsNan(Vector3Double p)
		{
			if (isnan(p.x) || isnan(p.y) || isnan(p.z))
				return true;

			return false;
		}
	}

	class InglesRayTracer2 : public IRayTracer
	{
	private:
		TraceParams m_traceParams;

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
		}

		void CalculateNextCycle(const Vector3Double& z, const Vector3Double& dz, Vector3Double& z1, Vector3Double& dz1)
		{
			z1.x = z.x * z.x - z.y * z.y - z.z * z.z + m_traceParams.Fractal.ConstantC.X;
			z1.y = 2.0 * z.x * z.y + m_traceParams.Fractal.ConstantC.Y;
			z1.z = 2.0 * z.x * z.z + m_traceParams.Fractal.ConstantC.Z;

			dz1.x = 2.0 * z.x * dz.x - z.y * dz.y - z.z * dz.z;
			dz1.y = z.y * dz.x + z.x + dz.y;
			dz1.z = z.x * dz.z + z.z * dz.x;
		}

		double EstimateDistance(Vector3Double& pos)
		{
			Vector3Double z = pos;
			Vector3Double dz(1.0, 1.0, 1.0);
			double dr = 1.0;
			double r = 0.0;
			Vector3Double z1;
			Vector3Double dz1;

			if (m_traceParams.Fractal.NormalizationType == BulbNormalizeType::AltRoots)
			{
				r = z.Length(m_traceParams.Fractal.NormalizationRoot);
			}
			else
			{
				r = z.Length();
			}

			int i = 0;
			for (; i < m_traceParams.Bulb.Iterations; ++i)
			{
				if (r > m_traceParams.Fractal.Bailout)
					break;

				CalculateNextCycle(z, dz, z1, dz1);

				if (IsNan(z1))
					break;

				if (m_traceParams.Fractal.NormalizationType == BulbNormalizeType::AltRoots)
				{
					r = z1.Length(m_traceParams.Fractal.NormalizationRoot);
					dr = dz1.Length(m_traceParams.Fractal.NormalizationRoot);
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


		double RayMarch(const Vector3Double& start, const Vector3Double& direction, Vector3Double& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; steps++)
			{
				p = totalDistance * direction + start;
				double distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.Bulb.StepDivisor;                      // note change 
				if (distance < m_traceParams.Bulb.MinRayDistance || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			return 1.0 - ((double)steps) / m_traceParams.Bulb.MaxRaySteps;
		}

		double RayMarchFractional(const Vector3Double& start, const Vector3Double& direction, Vector3Double& p)
		{
			double totalDistance = 0.0;
			double distance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; ++steps)
			{
				p = totalDistance * direction + start;
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

		double RayMarchDistance(const Vector3Double& start, const Vector3Double& direction, Vector3Double& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; steps++)
			{
				p = totalDistance * direction + start;
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
				Vector3Double pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v);

				Vector3Double dV = v;
				Vector3Double direction = -1.0 * dV;
				Vector3Double p;
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

		double CalculateStretchDistance(const Vector3Double& start, const Vector3Double& direction, Vector3Double& p, const StretchDistanceParams& stretchParams)
		{
			double distance = RayMarchDistance(start, direction, p);

			if (distance < stretchParams.MinDistance)
				return 0.0;

			if (distance > stretchParams.MaxDistance)
				return 1.0;

			return (distance - stretchParams.MinDistance) / (stretchParams.MaxDistance - stretchParams.MinDistance);
		}

	public:

		InglesRayTracer2(const TraceParams& traceParams)
			: m_traceParams(traceParams)
		{}

		virtual ~InglesRayTracer2()
		{}

		std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const std::function<void(double)>& setProgress) override
		{
			std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();
			vData->StretchParams = m_traceParams.Stretch;

			double total = data.Vertices.size() + 2.0;
			int progress = 1;

			std::function<double(const Vector3Double&, const Vector3Double&, Vector3Double&)> marcher = nullptr;

			if (m_traceParams.Bulb.Fractional)
				marcher = [this](const Vector3Double& start, const Vector3Double& direction, Vector3Double& p)
			{
				Vector3Double startD(start);
				Vector3Double directionD(direction);
				Vector3Double pD(p);
				double distance = RayMarchFractional(startD, directionD, pD);
				p = pD.ToVector3();

				return distance;
			};
			else
				marcher = [this](const Vector3Double& start, const Vector3Double& direction, Vector3Double& p)
			{
				Vector3Double startD(start);
				Vector3Double directionD(direction);
				Vector3Double pD(p);
				double distance = RayMarch(startD, directionD, pD);
				p = pD.ToVector3();

				return distance;
			};

			XMFLOAT3 nullNormal(0.0f, 0.0f, 0.0f);

			for (const XMFLOAT3& v : data.Vertices)
			{
				Vector3Double pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v);

				Vector3Double direction = -1.0 * Vector3Double(v);
				Vector3Double p;
				double distance = marcher(pt, direction, p);

				vData->Vertices.emplace_back(p.ToVector3(), nullNormal, Vector2(static_cast<float>(distance), 0.0f));

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
					setProgress(0.25 + 3.0 * progress / 4.0);
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
				Vector3Double pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v);

				Vector3Double direction = -1.0f * Vector3Double(v);
				Vector3Double p;
				double distance = CalculateStretchDistance(pt, direction, p, m_traceParams.Stretch);

				vData->Vertices.emplace_back(p.ToVector3(), nullNormal, Vector2(static_cast<float>(distance), 0.0f));

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

	std::unique_ptr<IRayTracer> CreateInglesRayTracer2(const TraceParams& traceParams)
	{
		return std::make_unique<InglesRayTracer2>(traceParams);
	}
}