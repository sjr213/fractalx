#include "stdafx.h"
#include "BulbNormalizeType.h"

#include <assert.h>

namespace DXF
{
	int BulbNormalizeTypeToInt(BulbNormalizeType normalizeType)
	{
		switch (normalizeType)
		{
		case BulbNormalizeType::StandardNormalization:
			return 1;
		case BulbNormalizeType::AltRoots:
			return 2;
		default:
			assert(false);
			return 1;
		}
	}

	BulbNormalizeType BulbNormalizeTypeFromInt(int type)
	{
		switch (type)
		{
		case 1:
			return BulbNormalizeType::StandardNormalization;
		case 2:
			return BulbNormalizeType::AltRoots;
		default:
			assert(false);
			return BulbNormalizeType::StandardNormalization;
		}
	}

	CString BulbNormalizeTypeString(BulbNormalizeType normalizeType)
	{
		switch (normalizeType)
		{
		case BulbNormalizeType::StandardNormalization:
			return _T("Standard Normalization");
		case BulbNormalizeType::AltRoots:
			return _T("Alt root normalization");
		default:
			assert(false);
			return _T("Unknown");
		}
	}
}