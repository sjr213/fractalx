#include "stdafx.h"
#include "RayTracerDoubleCommon.h"

#include <algorithm>
#include <DirectXMath.h>
#include "RayTracerCommon.h"
#include "SphereApproximator.h"
#include "TraceParams.h"
#include "Vector3Double.h"
#include <memory>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DXF
{
	bool IsNan(Vector3Double p)
	{
		if (isnan(p.x) || isnan(p.y) || isnan(p.z))
			return true;

		return false;
	}

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
			Vector3Double pt = MakeStartingPoint(traceParams.Bulb.Distance, traceParams.Bulb.Origin, v);

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

	class InternalDoubleRayTracer : public IRayTracer
	{
	private:
		std::function<double(Vector3Double&, const TraceParams&)> m_estimateDistance;
		TraceParams m_traceParams;

	protected:

		double RayMarch(const Vector3Double& start, const Vector3Double& direction, Vector3Double& p)
		{
			double totalDistance = 0.0;
			int steps;

			double lastDistance = std::numeric_limits<double>::max();

			for (steps = 0; steps < m_traceParams.Bulb.MaxRaySteps; steps++)
			{
				p = totalDistance * direction + start;
				double distance = m_estimateDistance(p, m_traceParams);
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
				distance = m_estimateDistance(p, m_traceParams);
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
				double distance = m_estimateDistance(p, m_traceParams);
				totalDistance += distance / m_traceParams.Bulb.StepDivisor;                      // note change 
				if (distance < m_traceParams.Bulb.MinRayDistance || distance > lastDistance)
					break;

				lastDistance = distance;
			}

			// We might want to set lastDistance to distance if it breaks on first step
			return lastDistance;
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

		StretchDistanceParams EstimateStretchRange(const TriangleData& data, const std::function<void(double)>& setProgress)
		{
			double minDistance = std::numeric_limits<double>::max();
			double maxDistance = 0.0;

			size_t nVertices = data.Vertices.size();

			for (size_t nVertex = 0; nVertex < nVertices; nVertex += 3)
			{
				const XMFLOAT3& v = data.Vertices.at(nVertex);
				Vector3Double pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v, m_traceParams.Fractal.ConstantC);

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

		std::shared_ptr<DxVertexData> RayTraceNormal(const TriangleData& data, const std::function<void(double)>& setProgress)
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

		std::shared_ptr<DxVertexData> RayTraceStepStretch(const TriangleData& data, const std::function<void(double)>& setProgress)
		{
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

			// new part for estimating distance
			auto setLocalProgress = setProgress;

			if (m_traceParams.Stretch.EstimateMinMax)
			{
				setLocalProgress = [&](double progress)
				{
					setProgress(0.25 + 3.0 * progress / 4.0);
				};

				m_traceParams.Stretch = EstimateStepStretchRange(marcher, data, [&](double progress)
					{
						setProgress(progress / 4.0);
					}, m_traceParams);
			}

			// new part 2
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
				double distance = CalculateStepStretchDistance(marcher, pt, direction, p, m_traceParams.Stretch);

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


	public:
		InternalDoubleRayTracer(const std::function<double(Vector3Double&, const TraceParams&)>& estimateDistance,
			const TraceParams& traceParams)
			: m_estimateDistance(estimateDistance)
			, m_traceParams(traceParams)
		{}

		virtual ~InternalDoubleRayTracer()
		{}

		std::shared_ptr<DxVertexData> RayTrace(const TriangleData& data, const std::function<void(double)>& setProgress) override
		{
			if (m_traceParams.Stretch.Stretch)
				return RayTraceStepStretch(data, setProgress);

			return RayTraceNormal(data, setProgress);
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
				Vector3Double pt = MakeStartingPoint(m_traceParams.Bulb.Distance, m_traceParams.Bulb.Origin, v, m_traceParams.Fractal.ConstantC);

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

	std::unique_ptr<IRayTracer> CreateInternalDoubleRayTracer(const TraceParams& traceParams,
		const std::function<double(Vector3Double&, const TraceParams&)>& estimateDistance)
	{
		return std::make_unique<InternalDoubleRayTracer>(estimateDistance, traceParams);
	}
}