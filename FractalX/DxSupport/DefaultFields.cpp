
#include "stdafx.h"
#include "DefaultFields.h"

namespace DXF
{
	Vertex<float> GetDefaultWorldScale()
	{
		return Vertex<float>(0.1f, 0.1f, 0.1f);
	}

	Vertex<float> GetDefaultTarget()
	{
		return Vertex<float>(0.0f, 0.0f, 0.0f);
	}

	Vertex<float> GetDefaultCamera()
	{
		return Vertex<float>(0.f, 0.f, 0.3f);
	}

	double GetMaxStretch()
	{
		return 1e7;
	}

	std::vector<Vertex<float>> GetDefaultBackgroundModel()
	{
		std::vector<Vertex<float>> model;
		model.emplace_back(-4.0f, -2.0f, -4.0f);
		model.emplace_back(-4.0f, -2.0f, 4.0f);
		model.emplace_back(4.0f, -2.0f, 4.0f);
		model.emplace_back(4.0f, -2.0f, -4.0f);

		return model;
	}
}