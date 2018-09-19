#include "stdafx.h"
#include "TriangleLoader.h"

#include <afxwin.h>
#include "DxSerialize.h"
#include "ModelData.h"
#include "SphereApproximator.h"

namespace DXF
{
	namespace TriangleLoader
	{
		CString GetTrianglePath()
		{
			TCHAR szPath[_MAX_PATH];
			DWORD len = ::GetModuleFileName(AfxGetApp()->m_hInstance, szPath, _MAX_PATH);
			if (len == 0)
				return CString();

			CString csPath(szPath);
			int nIndex = csPath.ReverseFind(_T('\\'));

			if (nIndex > 0)
				csPath = csPath.Left(nIndex);
			else
				return CString();

			csPath += _T("\\Triangles\\");
			return csPath;
		}

		CString GetFileName(const ModelData& modelData)
		{
			switch (modelData.VertexIterations)
			{
			case 5:
				if (modelData.TriangleSeeds == SeedTriangles::Eight)
					return _T("triangles5_8.triangles");
				break;
			case 6:
				if (modelData.TriangleSeeds == SeedTriangles::Eight)
					return _T("triangles6_8.triangles");
				break;
			case 7:
				if (modelData.TriangleSeeds == SeedTriangles::Eight)
					return _T("triangles7_8.triangles");
				break;
			case 8:
				if (modelData.TriangleSeeds == SeedTriangles::Eight)
					return _T("triangles8_8.triangles");
				break;
			case 9:
				if (modelData.TriangleSeeds == SeedTriangles::Eight)
					return _T("triangles9_8.triangles");
				break;
			case 10:
				if (modelData.TriangleSeeds == SeedTriangles::Eight)
					return _T("triangles10_8.triangles");
				break;
			}

			return CString();
		}

		std::shared_ptr<TriangleData> ReadTriangleFile(CString pathName)
		{
			// Create a CFile and then a CArchive
			CFile triangleFile;

			// do something about exceptions
			CFileException FileExcept;
			if (!triangleFile.Open(pathName, CFile::modeRead, &FileExcept))
			{
				return nullptr;
			}

			CArchive ar(&triangleFile, CArchive::load);

			std::shared_ptr<TriangleData> triangles = std::make_shared<TriangleData>();
			Serialize(ar, *triangles);

			ar.Close();
			triangleFile.Close();

			return triangles;
		}
	
		std::shared_ptr<TriangleData> LoadTriangles(const ModelData& modelData)
		{
			CString path = GetTrianglePath();
			if (path.IsEmpty())
				return nullptr;

			CString fileName = GetFileName(modelData);
			if (fileName.IsEmpty())
				return nullptr;

			path += fileName;

			return ReadTriangleFile(path);
		}


		std::shared_ptr<TriangleData> GetTriangles(const ModelData& modelData, const std::function<void(double)>& setProgress)
		{
			auto triangles = LoadTriangles(modelData);
			if (triangles)
			{
				setProgress(1.0);
				return triangles;
			}

			std::shared_ptr<TriangleData> tData = GenerateCrudeTriangles(modelData.VertexIterations, modelData.TriangleSeeds, [&](double progress)
			{
				setProgress(progress);
			});

			NormalizeVectors(*tData);

			return tData;
		}

		std::shared_ptr<TriangleData> CreateTriangles(const ModelData& modelData, const std::function<void(double)>& setProgress)
		{
			std::shared_ptr<TriangleData> tData = GenerateCrudeTriangles(modelData.VertexIterations, modelData.TriangleSeeds, [&](double progress)
			{
				setProgress(progress);
			});

			NormalizeVectors(*tData);

			return tData;
		}
	}
}