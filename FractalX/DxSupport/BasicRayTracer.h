#pragma once

#include "IRayTracer.h"

namespace DXF
{
	std::shared_ptr<IRayTracer> CreateBasicRayTracer(const TraceParams& traceParams);
}