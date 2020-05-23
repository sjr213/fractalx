
#pragma once

#include "ModelData.h"
#include <optional>
#include "Perspective.h"
#include <tuple>
#include "Vertex.h"
#include "VertexData.h"
#include <string>

class CDC;

namespace DXF
{
	struct DxEffectColors;
	struct RotationParams;
	struct DxLights;

	class Renderer
	{
	public:
		static std::shared_ptr<Renderer> CreateRenderer();

		virtual void SetModel(const DxVertexData& vertexData) = 0;

		virtual void SetModel2(const DxVertexData& vertexData2, const std::wstring& textureFile) = 0;

		virtual void ResetModel() = 0;

		virtual void ResetModel2() = 0;

		// Initialization and management
		virtual void Initialize(HWND window, int width, int height) = 0;

		virtual void SetRotationParams(const RotationParams& rp) = 0;

		virtual RotationParams GetRotationParams() = 0;

		// Basic game loop
		virtual void Tick() = 0;

		virtual void RefreshRender(float time) = 0;

		// Messages
		virtual void OnWindowSizeChanged(int width, int height) = 0;

		// Properties
		virtual void GetDefaultSize(int& width, int& height) const = 0;

		virtual void GetWindowSize(int& width, int& height) const = 0;

		virtual void SetTextureColors(std::vector<uint32_t> colors) = 0;

		virtual void SetPerspective(const DxPerspective& perspective) = 0;

		virtual DxPerspective GetPerspective() const = 0;

		virtual void SetView(const Vertex<float>& camera, const Vertex<float>& target) = 0;

		virtual void SetTarget(const Vertex<float>& target) = 0;

		virtual Vertex<float> GetCamera() const = 0;
		virtual Vertex<float> GetTarget() const = 0;

		virtual bool IsReady() = 0;

		virtual CSize GetScreenSize() const = 0;

		virtual bool DrawImage(CDC& dc, CSize imageSize) = 0;

		virtual void SetBackgroundColor(const DirectX::SimpleMath::Color& bkColor) = 0;

		virtual void SetEffectColors(DxEffectColors& effectColors) = 0;

		virtual void SetLights(DxLights& lights) = 0;

		virtual std::optional<DXF::Vertex<float>> Map2Dto3D(int x, int y) = 0;

		virtual void SetWorldScale(const DXF::Vertex<float>& scale) = 0;

		virtual Vertex<float> GetWorldScale() const = 0;

	protected:
		Renderer() {}
	};
}
