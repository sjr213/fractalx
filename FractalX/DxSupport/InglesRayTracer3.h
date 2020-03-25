#pragma once

#include "IRayTracer.h"

namespace DXF
{
	std::unique_ptr<IRayTracer> CreateInglesRayTracer3(const TraceParams& traceParams);
}
