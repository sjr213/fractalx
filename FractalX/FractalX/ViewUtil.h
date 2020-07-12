#pragma once

class CFractalXDoc;

namespace DXF
{
	class Renderer;
}

namespace ViewUtil
{
	bool EditPerspective(CFractalXDoc& doc, DXF::Renderer& renderer);

	bool EditView(CFractalXDoc& doc, DXF::Renderer& renderer);

	bool EditBackgroundColor(CFractalXDoc& doc, DXF::Renderer& renderer, CWnd* pWnd);

	void MoveModel(CFractalXDoc& doc, DXF::Renderer& renderer, int deltaX, int deltaY);

	void MoveModelAlongZAxis(CFractalXDoc& doc, DXF::Renderer& renderer, int deltaZ);

	void OnExportTriangles();

	bool EditEffectColors(CFractalXDoc& doc, DXF::Renderer& renderer, CWnd* pWnd);

	bool EditLights(CFractalXDoc& doc, DXF::Renderer& renderer, CWnd* pWnd);
}