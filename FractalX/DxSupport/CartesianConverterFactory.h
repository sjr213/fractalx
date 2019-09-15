#pragma once

#include <functional>
#include "Vector3Double.h"

namespace DXF
{
	struct CartesianConverterGroup;

	std::function<void(Vector3Double&, const double&, const double&, const double&)> CreateConverter(const CartesianConverterGroup& requestParams);
}