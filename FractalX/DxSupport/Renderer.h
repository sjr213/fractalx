
#pragma once

#include "ModelData.h"
#include <optional>
#include "Perspective.h"
#include <tuple>
#include "VertexData.h"

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

		virtual void ResetModel() = 0;

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

		virtual void SetTextureColors(std::vector<uint32_t> colors) = 0;

		virtual void SetPerspective(const DxPerspective& perspective) = 0;

		virtual DxPerspective GetPerspective() const = 0;

		virtual void SetView(const std::tuple<float, float, float>& camera, const std::tuple<float, float, float>& target) = 0;

		virtual void SetTarget(const std::tuple<float, float, float>& target) = 0;

		virtual std::tuple<float, float, float> GetCamera() const = 0;

		virtual std::tuple<float, float, float> GetTarget() const = 0;

		virtual bool IsReady() = 0;

		virtual CSize GetScreenSize() const = 0;

		virtual bool DrawImage(CDC& dc, CSize imageSize) = 0;

		virtual void SetBackgroundColor(const DirectX::SimpleMath::Color& bkColor) = 0;

		virtual void SetEffectColors(DxEffectColors& effectColors) = 0;

		virtual void SetLights(DxLights& lights) = 0;

		virtual std::optional<std::tuple<float, float, float>> Map2Dto3D(int x, int y) = 0;

	protected:
		Renderer() {}
	};
}
