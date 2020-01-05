#pragma once

#include "IRayTracer.h"

namespace DXF
{
	std::unique_ptr<IRayTracer> CreateInglesRayTracer(const TraceParams& traceParams);
}