#pragma once

#include "VertexData.h"

namespace DXF
{
	struct DxBackgroundVertexData
	{
		DxBackgroundVertexData()
			: Show(false)
			, VertexData(new DxVertexData)
		{}

		DxBackgroundVertexData(bool show, const DxVertexData& vData, std::wstring filename)
			: Show(show)
			, VertexData(std::make_shared<DxVertexData>(vData))
			, Filename(filename)
		{}

		DxBackgroundVertexData(const DxBackgroundVertexData& data)
		{
			Show = data.Show;
			*VertexData = *data.VertexData;
			Filename = data.Filename;
		}

		void operator=(const DxBackgroundVertexData& data)
		{
			Show = data.Show;
			*VertexData = *data.VertexData;
			Filename = data.Filename;
		}

		bool Show;
		std::shared_ptr<DxVertexData> VertexData;
		std::wstring Filename;
	};
}
