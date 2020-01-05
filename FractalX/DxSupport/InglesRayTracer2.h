#pragma once

#include "IRayTracer.h"

namespace DXF
{
	std::unique_ptr<IRayTracer> CreateInglesRayTracer2(const TraceParams& traceParams);
}