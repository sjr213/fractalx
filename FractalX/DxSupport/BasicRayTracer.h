#pragma once

#include "IRayTracer.h"

namespace DXF
{
	std::unique_ptr<IRayTracer> CreateBasicRayTracer(const TraceParams& traceParams);
}