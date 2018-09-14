#pragma once


namespace DXF
{

	struct StretchDistanceParams;
	struct FractalParams;
	struct BulbParams;
	struct TraceParams;

	void Serialize(CArchive& ar, StretchDistanceParams& stretchParams);

	void Serialize(CArchive& ar, FractalParams& fractalParams);

	void Serialize(CArchive& ar, BulbParams& bulbParams);

	void Serialize(CArchive& ar, TraceParams& traceParams);

}