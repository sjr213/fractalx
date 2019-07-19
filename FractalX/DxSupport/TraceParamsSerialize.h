#pragma once


namespace DXF
{
	struct StretchDistanceParams;
	struct FractalParams;
	struct BulbParams;
	struct TraceParams;
	struct TriangleData;

	void Serialize(CArchive& ar, StretchDistanceParams& stretchParams);

	void Serialize(CArchive& ar, FractalParams& fractalParams);

	void Serialize(CArchive& ar, BulbParams& bulbParams);

	void Serialize(CArchive& ar, TraceParams& traceParams);

	void Serialize(CArchive& ar, TriangleData& triangles);

}