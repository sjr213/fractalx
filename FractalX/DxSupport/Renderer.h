
#pragma once

#include "ModelData.h"
#include "VertexData.h"
#include <tuple>

namespace Gdiplus
{
	class Bitmap;
}

namespace DXF
{
	struct RotationParams;

	class Renderer
	{
	public:
		static std::shared_ptr<Renderer> CreateRenderer();

		virtual void SetModel(const DxVertexData& vertexData) = 0;

		virtual void ResetModel() = 0;

		// Initialization and management
		virtual void Initialize(HWND window, int width, int height) = 0;

		virtual void SetRotationParams(const RotationParams& rp) = 0;

		// Basic game loop
		virtual void Tick() = 0;

		virtual void RefreshRender(float time) = 0;

		// Messages
		virtual void OnWindowSizeChanged(int width, int height) = 0;

		// Properties
		virtual void GetDefaultSize(int& width, int& height) const = 0;

		virtual void SetTextureColors(std::vector<uint32_t> colors) = 0;

		virtual void SetPerspective(float near, float far) = 0;

		virtual float GetNear() const = 0;

		virtual float GetFar() const = 0;

		virtual void SetView(const std::tuple<float, float, float>& camera, const std::tuple<float, float, float>& target) = 0;

		virtual void SetTarget(const std::tuple<float, float, float>& target) = 0;

		virtual std::tuple<float, float, float> GetCamera() const = 0;

		virtual std::tuple<float, float, float> GetTarget() const = 0;

		virtual bool IsReady() = 0;

		virtual std::shared_ptr<Gdiplus::Bitmap> GrabImage() = 0;

	protected:
		Renderer() {}
	};
}
