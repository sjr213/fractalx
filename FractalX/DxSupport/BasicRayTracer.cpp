
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
	namespace
	{
		DirectX::XMFLOAT3 MakeStartingPoint(float distance, const XMFLOAT3& origin, const XMFLOAT3& direction)
		{
			return distance * direction + origin;
		}

		DirectX::XMFLOAT3 CrossProduct(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			Vector3 n(v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x);

			n.Normalize();

			return n;
		}

		// From Luna DirectX 11, 7.2.1
		void CalculateNormals(DxVertexData& data)
		{
			size_t nTriangles = data.Indices.size() / 3;

			for (size_t i = 0; i < nTriangles; ++i)
			{
				unsigned int i0 = data.Indices[i * 3 + 0];
				unsigned int i1 = data.Indices[i * 3 + 1];
				unsigned int i2 = data.Indices[i * 3 + 2];

				DirectX::VertexPositionNormalTexture v0 = data.Vertices[i0];
				DirectX::VertexPositionNormalTexture v1 = data.Vertices[i1];
				DirectX::VertexPositionNormalTexture v2 = data.Vertices[i2];

				// compute face normals
				DirectX::XMFLOAT3 e0 = v1.position - v0.position;
				DirectX::XMFLOAT3 e1 = v2.position - v0.position;
				DirectX::XMFLOAT3 faceNormal = CrossProduct(e0, e1);

				// This triangle shares the following three vertices.
				// so add this face normal into the average of these
				// vertex normals.
				data.Vertices[i0].normal = data.Vertices[i0].normal + faceNormal;
				data.Vertices[i1].normal = data.Vertices[i1].normal + faceNormal;
				data.Vertices[i2].normal = data.Vertices[i2].normal + faceNormal;
			}

			// normalize all vertex normals
			for (DirectX::VertexPositionNormalTexture& vertex : data.Vertices)
			{
				Vector3 v = vertex.normal;
				v.Normalize();
				vertex.normal = v;
			}
		}

		// Cartesian converters
		void StandardCartesianConverter(XMFLOAT3& z, const double& zr, const double& theta, const double& phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(sin(theta) * cos(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(cos(theta)));
		}

		void CartesianConverterAltX1(XMFLOAT3& z, const double& zr, const double& theta, const double& phi)
		{
			z = static_cast<float>(zr) * XMFLOAT3(static_cast<float>(sin(theta) * tan(phi)), static_cast<float>(sin(phi) * sin(theta)), static_cast<float>(cos(theta)));
		}
	}

	class BasicRayTracer : public IRayTracer
	{
	private:
		TraceParams m_traceParams;
		std::function<void(XMFLOAT3&, const double&, const double&, const double&)> m_cartesianConverter;

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

		double EstimateDistance(XMFLOAT3& pos)
		{
			Vector3 z = pos;
			double dr = 1.0;
			double r = 0.0;

			for (int i = 0; i < m_traceParams.Bulb.Iterations; ++i)
			{
				r = z.Length();
				if (r > m_traceParams.Fractal.Bailout)
					break;

				CalculateNextCycle(z, r, dr);

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

	std::shared_ptr<IRayTracer> CreateBasicRayTracer(const TraceParams& traceParams)
	{
		return std::make_shared<BasicRayTracer>(traceParams, StandardCartesianConverter);
	}
}