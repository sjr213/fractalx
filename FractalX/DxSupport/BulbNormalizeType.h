#pragma once

namespace DXF
{
	enum class BulbNormalizeType
	{
		StandardNormalization = 1,
		AltRoots = 2,
	};

	int BulbNormalizeTypeToInt(BulbNormalizeType normalizeType);

	BulbNormalizeType BulbNormalizeTypeFromInt(int type);

	CString BulbNormalizeTypeString(BulbNormalizeType normalizeType);
}