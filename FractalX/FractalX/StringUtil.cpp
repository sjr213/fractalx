#include "stdafx.h"
#include "StringUtil.h"

#include <iosfwd>
#include <sstream>

namespace StringUtil
{
	CString MakeStatusMessage(CPoint start, CPoint end, std::optional<DXF::Vertex<float>> startVertex, std::optional<DXF::Vertex<float>> endVertex)
	{
		std::wstringstream ss;
		ss << L'(' << start.x << L", " << start.y << L") ";
		if (startVertex.has_value())
		{
			ss << L'[' << startVertex->X << L", " << startVertex->Y << L", " << startVertex->Z << L"] ";
		}
		ss << L"to ";
		ss << L'(' << end.x << L", " << end.y << L')';
		if (endVertex.has_value())
		{
			ss << L'[' << endVertex->X << L", " << endVertex->Y << L", " << endVertex->Z << L"] ";
		}

		return ss.str().c_str();
	}

	CString Concat(const CString& str1, const CString& str2, const CString& str3, const CString& str4)
	{
		std::wstringstream ss;
		ss << str1.GetString() << str2.GetString() << str3.GetString() << str4.GetString();

		return ss.str().c_str();
	}

	CString Concat(const CString& str1, const CString& str2, const CString& str3, const CString& str4, const CString& str5)
	{
		std::wstringstream ss;
		ss << str1.GetString() << str2.GetString() << str3.GetString() << str4.GetString() << str5.GetString();

		return ss.str().c_str();
	}
}