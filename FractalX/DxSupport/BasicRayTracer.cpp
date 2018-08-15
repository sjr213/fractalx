
#include "stdafx.h"
#include "BasicRayTracer.h"

#include "SphereApproximator.h"
#include "TraceParams.h"
#include "VertexData.h"
#include <SimpleMath.h>
#include <limits>
#include "VectorHelpers.h"
#include <algorithm>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	static DirectX::SimpleMath::Vector3 MakeStartingPoint(float distance, const Vector3& origin, const Vector3& direction)
	{
		return distance * direction + origin;
	}

	class BasicRayTracerImpl : public BasicRayTracer
	{
	private:
		TraceParams m_traceParams;

	protected:

		void CalculateNextCycle(Vector3& z, double& r, double& dr)
		{
			// convert to polar coordinates
			double theta = acos(z.z / r);
			double phi = atan2(z.y, z.x);

			dr = pow(r, m_traceParams.Power - 1.0) *  m_traceParams.Power * dr + m_traceParams.ConstantC;

			// scale and rotate the point
			double zr = pow(r, m_traceParams.Power);
			theta = theta * m_traceParams.Power;
			phi = phi * m_traceParams.Power;

			// convert back to Cartesian coordinates
			z = static_cast<float>(zr) * Vector3(static_cast<float>(sin(theta) * cos(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(cos(theta)));
		}

		double EstimateDistance(Vector3& pos)
		{
			Vector3 z = pos;
			double dr = 1.0;
			double r = 0.0;

			for (int i = 0; i < m_traceParams.Iterations; ++i)
			{
				r = z.Length();
				if (r > m_traceParams.Bailout)
					break;

				CalculateNextCycle(z, r, dr);

				z += pos;
			}

			if (r == 0.0 || dr == 0.0)
				return 0.0;

			return 0.5 * log(r) * r / dr;
		}


		double RayMarch(const Vector3& start, const Vector3& direction, Vector3& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.MaxRaySteps; steps++)
			{
				p = static_cast<float>(totalDistance) * direction + start;
				double distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.StepDivisor;                      // note change 
				if (distance < m_traceParams.MinRayDistance || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			return 1.0 - ((double)steps) / m_traceParams.MaxRaySteps;
		}

		double RayMarchFractional(const Vector3& start, const Vector3& direction, Vector3& p)
		{
			double totalDistance = 0.0;
			double distance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.MaxRaySteps; ++steps)
			{
				p = static_cast<float>(totalDistance) * direction + start;
				distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.StepDivisor;                     
				if (distance < m_traceParams.MinRayDistance) // || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			double delta = steps;
			if (steps < m_traceParams.MaxRaySteps - 1 && distance < m_traceParams.MinRayDistance)
				delta = steps + distance / m_traceParams.MinRayDistance;

			delta = std::min(delta, static_cast<double>(m_traceParams.MaxRaySteps));

			return 1.0 - delta / m_traceParams.MaxRaySteps;
		}

		double RayMarchDistance(const Vector3& start, const Vector3& direction, Vector3& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.MaxRaySteps; steps++)
			{
				p = static_cast<float>(totalDistance) * direction + start;
				double distance = EstimateDistance(p);
				totalDistance += distance / m_traceParams.StepDivisor;                      // note change 
				if (distance < m_traceParams.MinRayDistance || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			// We might want to set lastDistance to distance if it breaks on first step
			return lastDistance;
		}

		Vector3 CalculateNormal(const Vector3& pos, float normalDelta)
		{
			double plusX = EstimateDistance(AddScaler(pos, normalDelta));
			double minusX = EstimateDistance(AddScaler(pos, -1.0f * normalDelta));
			double plusY = EstimateDistance(AddScaler(pos, normalDelta));
			double minusY = EstimateDistance(AddScaler(pos, -1.0f * normalDelta));
			double plusZ = EstimateDistance(AddScaler(pos, normalDelta));
			double minusZ = EstimateDistance(AddScaler(pos, -1.0f * normalDelta));

			Vector3 norm(static_cast<float>(plusX - minusX), static_cast<float>(plusY - minusY), static_cast<float>(plusZ - minusZ));
			norm.Normalize();
			return norm;
		}

		StretchDistanceParams EstimateStretchRange(const TriangleData& data, const std::function<void(double)>& setProgress)
		{
			double minDistance = std::numeric_limits<double>::max();
			double maxDistance = 0.0;

			size_t nVertices = data.Vertices.size();

			for (size_t nVertex = 0; nVertex < nVertices; nVertex += 3)
			{
				const Vector3& v = data.Vertices.at(nVertex);
				Vector3 pt = MakeStartingPoint(m_traceParams.Distance, m_traceParams.Origin, v);

				Vector3 direction = -1.0f * v;
				Vector3 p;
				double distance = RayMarchDistance(pt, direction, p);

				minDistance = std::min(minDistance, distance);
				maxDistance = std::max(maxDistance, distance);

				if (nVertex % 30)
					setProgress(static_cast<double>(nVertex) / nVertices);
			}

			StretchDistanceParams stretchParams = m_traceParams.StretchParams;
			stretchParams.MinDistance = minDistance;
			stretchParams.MaxDistance = maxDistance;

			return stretchParams;
		}

		double CalculateStretchDistance(const Vector3& start, const Vector3& direction, Vector3& p, const StretchDistanceParams& stretchParams)
		{
			double distance = RayMarchDistance(start, direction, p);

			if (distance < stretchParams.MinDistance)
				return 0.0;

			if (distance > stretchParams.MaxDistance)
				return 1.0;

			return (distance - stretchParams.MinDistance) / (stretchParams.MaxDistance - stretchParams.MinDistance);
		}

	public:

		std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const TraceParams& traceParams, 
			const std::function<void(double)>& setProgress) override
		{
			m_traceParams = traceParams;

			std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();
			vData->StretchParams = traceParams.StretchParams;

			double total = data.Vertices.size() + 2.0;
			int progress = 1;

			std::function<double(const Vector3&, const Vector3&, Vector3&)> marcher = 
				[this](const Vector3& start, const Vector3& direction, Vector3& p)
			{
				return RayMarch(start, direction, p);
			};
			if (traceParams.Fractional)
				marcher = [this](const Vector3& start, const Vector3& direction, Vector3& p)
			{
				return RayMarchFractional(start, direction, p);
			};

			for (const Vector3& v : data.Vertices)
			{
				Vector3 pt = MakeStartingPoint(traceParams.Distance, traceParams.Origin, v);

				Vector3 direction = -1.0f * v;
				Vector3 p;
				double distance = marcher(pt, direction, p);

				auto normal = CalculateNormal(p, m_traceParams.NormalDelta);

				vData->Vertices.emplace_back(p, normal, Vector2(static_cast<float>(distance), 0.0f));

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

			setProgress(1.0);

			return vData;
		}

		std::shared_ptr<DxVertexData> RayTraceStretch(const TriangleData& data, const TraceParams& traceParams,
			const std::function<void(double)>& setProgress) override
		{
			m_traceParams = traceParams;

			auto setLocalProgress = setProgress;

			if (m_traceParams.StretchParams.EstimateMinMax)
			{
				setLocalProgress = [&](double progress)
				{
					setProgress(0.25 + 3.0*progress / 4.0);
				};

				m_traceParams.StretchParams = EstimateStretchRange(data, [&](double progress)
				{
					setProgress(progress / 4.0);
				});
			}
				
			std::shared_ptr<DxVertexData> vData = std::make_shared<DxVertexData>();
			vData->StretchParams = m_traceParams.StretchParams;

			double total = data.Vertices.size() + 2.0;
			int progress = 1;

			for (const Vector3& v : data.Vertices)
			{
				Vector3 pt = MakeStartingPoint(m_traceParams.Distance, m_traceParams.Origin, v);

				Vector3 direction = -1.0f * v;
				Vector3 p;
				double distance = CalculateStretchDistance(pt, direction, p, m_traceParams.StretchParams);

				auto normal = CalculateNormal(p, m_traceParams.NormalDelta);

				vData->Vertices.emplace_back(p, normal, Vector2(static_cast<float>(distance), 0.0f));

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

			setProgress(1.0);

			return vData;
		}

	};


	std::shared_ptr<BasicRayTracer> BasicRayTracer::CreateBasicRayTracer()
	{
		return std::make_shared<BasicRayTracerImpl>();
	}
}