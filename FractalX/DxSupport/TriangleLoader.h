#pragma once


namespace DXF
{
	struct ModelData;
	struct TriangleData;

	namespace TriangleLoader
	{
		std::shared_ptr<TriangleData> GetTriangles(const ModelData& modelData, const std::function<void(double)>& setProgress);

		std::shared_ptr<TriangleData> CreateTriangles(const ModelData& modelData, const std::function<void(double)>& setProgress);
	}
}