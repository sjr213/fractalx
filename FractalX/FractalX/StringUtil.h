#pragma once

#include "Vertex.h"

namespace StringUtil
{
	CString MakeStatusMessage(CPoint start, CPoint end, std::optional<DXF::Vertex<float>> startVertex, std::optional<DXF::Vertex<float>> endVertex);

	CString Concat(const CString& str1, const CString& str2, const CString& str3, const CString& str4);

	CString Concat(const CString& str1, const CString& str2, const CString& str3, const CString& str4, const CString& str5);
}
