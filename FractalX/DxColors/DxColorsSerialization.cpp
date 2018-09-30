#include "stdafx.h"
#include "DxColorsSerialization.h"

#include <afx.h>
#include <assert.h>
#include "ColorContrast.h"


namespace DxColor
{
	namespace Serialization
	{
		void Serialize(CArchive& ar, ColorContrast& contrast)
		{
			const int ContrastVersion = 1;

			if (ar.IsStoring())
			{
				ar << ContrastVersion;
				ar << static_cast<int>(contrast.Mode);
				ar << contrast.MinContrast[0] << contrast.MinContrast[1] << contrast.MinContrast[2];
				ar << contrast.MaxContrast[0] << contrast.MaxContrast[1] << contrast.MaxContrast[2];

				ar << contrast.MinHue << contrast.MaxHue;
				ar << contrast.MinSaturation << contrast.MaxSaturation;
				ar << contrast.MinLightness << contrast.MaxLightness;
			}
			else
			{
				int version = 0;
				ar >> version;

				assert(version == ContrastVersion);
				if (version != ContrastVersion)
					return;

				int mode = 1;
				ar >> mode;
				ar >> contrast.MinContrast[0] >> contrast.MinContrast[1] >> contrast.MinContrast[2];
				ar >> contrast.MaxContrast[0] >> contrast.MaxContrast[1] >> contrast.MaxContrast[2];

				ar >> contrast.MinHue >> contrast.MaxHue;
				ar >> contrast.MinSaturation >> contrast.MaxSaturation;
				ar >> contrast.MinLightness >> contrast.MaxLightness;

				contrast.Mode = static_cast<ContrastType>(mode);
			}
		}

	}
}