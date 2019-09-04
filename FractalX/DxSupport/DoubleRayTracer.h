#pragma once

#include "IRayTracer.h"

namespace DXF
{
	std::unique_ptr<IRayTracer> CreateDoubleRayTracer(const TraceParams& traceParams);
}