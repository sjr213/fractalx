
#include "stdafx.h"
#include "ViewUtil.h"

#include "ColorSelectorDlg.h"
#include "ColorUtils.h"
#include "ConversionUtil.h"
#include "DxSerialize.h"
#include "EffectColorDlg.h"
#include "FractalXDoc.h"
#include "LightsDlg.h"
#include "ModelData.h"
#include "PerspectiveDlg.h"
#include "Renderer.h"
#include "SphereApproximator.h"
#include "SeedTriangles.h"
#include "StringUtil.h"
#include "TriangleLoader.h"
#include "ViewDlg.h"
#include "ColorPin.h"

namespace ViewUtil
{
	bool EditPerspective(CFractalXDoc& doc, DXF::Renderer& renderer)
	{
		auto perspective = doc.GetPerspective();

		CPerspectiveDlg dlg;
		dlg.SetNear(perspective.NearPlane);
		dlg.SetFarDistance(perspective.FarPlane);

		if (dlg.DoModal() == IDOK)
		{
			perspective.NearPlane = dlg.GetNearDistance();
			perspective.FarPlane = dlg.GetFarDistance();

			renderer.SetPerspective(perspective);

			// Get the perspective back from the renderer in case it changed anything
			doc.SetPerspective(renderer.GetPerspective());

			return true;
		}

		return false;
	}

	bool EditView(CFractalXDoc& doc, DXF::Renderer& renderer)
	{
		auto camera = doc.GetCamera();
		auto target = doc.GetTarget();

		CViewDlg dlg;
		dlg.SetCamera(camera);
		dlg.SetTarget(target);

		if (dlg.DoModal() == IDOK)
		{
			camera = dlg.GetCamera();
			target = dlg.GetTarget();

			doc.SetCamera(camera);
			doc.SetTarget(target);

			renderer.SetView(camera, target, doc.GetTargetBackground());
			return true;
		}

		return false;
	}

	bool EditBackgroundColor(CFractalXDoc& doc, DXF::Renderer& renderer, CWnd* pWnd)
	{
		auto bkColor = doc.GetBackgroundColor();

		std::shared_ptr<CColorSelectorDlg> pColorDlg = CColorSelectorDlg::CreateColorSelectorDlg(bkColor, pWnd);

		if (pColorDlg->DoModal() == IDOK)
		{
			auto newColor = pColorDlg->GetColor();
			doc.SetBackgroundColor(newColor);
			renderer.SetBackgroundColor(ColorUtils::ConvertToDxColor(newColor));

			//ForceModelRender();

			return true;
		}
		return false;
	}

	void MoveModel(CFractalXDoc& doc, DXF::Renderer& renderer, int deltaX, int deltaY)
	{
		constexpr float MouseDistance = 200.0f;

		auto target = doc.GetTarget();

		target.X = target.X + static_cast<float>(deltaX) / MouseDistance;
		target.Y = target.Y + static_cast<float>(deltaY) / MouseDistance;

		doc.SetTarget(target);
		renderer.SetTarget(target);
	}

	void MoveModelAlongZAxis(CFractalXDoc& doc, DXF::Renderer& renderer, int deltaZ)
	{
		constexpr float MouseDistanceZ = 50.0f;

		auto target = doc.GetTarget();

		target.Z = target.Z + static_cast<float>(deltaZ) / MouseDistanceZ;

		doc.SetTarget(target);
		renderer.SetTarget(target);
	}

	void OnExportTriangles()
	{
		CString fileName = _T("test1");
		CString fileExt(_T("triangles"));
		CString fileType;
		fileType = StringUtil::Concat(CString(L"triangle files (*"), fileExt, CString(L")|*"), fileExt, CString(L"|ALL Files |*.*||"));

		// Open a Save As dialog to get a name
		CFileDialog exportDlg(FALSE, fileExt, fileName, OFN_PATHMUSTEXIST, fileType);

		// Display as modal 
		if (exportDlg.DoModal() != IDOK)
			return;

		fileName = exportDlg.GetPathName();

		// Create a CFile and then a CArchive
		CFile triFile;

		// do something about exceptions
		CFileException FileExcept;
		if (!triFile.Open(fileName, CFile::modeCreate | CFile::modeWrite, &FileExcept))
		{
			CString error;
			wchar_t message[100] = { 0 };
			FileExcept.GetErrorMessage(message, 100);
			error = StringUtil::Concat(CString(L"Error opening file: "), fileName, CString(L": "), message);
			AfxMessageBox(error, MB_ICONWARNING);
			return;
		}

		// Get Triangles
		DXF::ModelData model;
		model.VertexIterations = 10;
		model.TriangleSeeds = DXF::SeedTriangles::Four;

		std::shared_ptr<DXF::TriangleData> triangles = DXF::TriangleLoader::CreateTriangles(model, [&](double /*progress*/) {});

		CArchive ar(&triFile, CArchive::store);

		DXF::Serialize(ar, *triangles);

		ar.Close();
		triFile.Close();

		AfxMessageBox(_T("Done exporting"));
	}

	bool EditEffectColors(CFractalXDoc& doc, DXF::Renderer& renderer, CWnd* pWnd)
	{
		auto effectColors = doc.GetEffectColors();
		auto colorsDlg = CEffectColorDlg::CreateEffectColorDlg(effectColors, pWnd);

		if (colorsDlg->DoModal() == IDOK)
		{
			effectColors = colorsDlg->GetColors();
			doc.SetEffectColors(effectColors);
			renderer.SetEffectColors(ConversionUtil::ConvertEffectColors(effectColors));
			return true;
		}

		return false;
	}

	bool EditLights(CFractalXDoc& doc, DXF::Renderer& renderer, CWnd* pWnd)
	{
		auto lights = doc.GetLights();
		auto lightsDlg = CLightsDlg::CreateLightsDlg(lights, pWnd);
		if (lightsDlg->DoModal())
		{
			lights = lightsDlg->GetLights();
			doc.SetLights(lights);
			renderer.SetLights(ConversionUtil::ConvertLights(lights));
			return true;
		}

		return false;
	}

}