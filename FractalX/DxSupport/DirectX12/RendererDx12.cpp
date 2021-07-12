
#include "stdafx.h"
#include "Renderer.h"

#include <afxwin.h>
//#include <algorithm>
//#include "BackgroundVertexData.h"
//#include "CommonStates.h"
//#include <d3d11_1.h>
//#include "DefaultFields.h"
#include <DirectXColors.h>
//#include "DirectXHelpers.h"
//#include "DirectXPackedVector.h"
//#include "DxfColorFactory.h"
#include "DxEffectColors.h"
#include "DxException.h"
#include <wrl/client.h>
//#include "DxFactoryMethods.h"
#include "DxLight.h"
#include <dxgi.h>
//#include <dxgi1_2.h>
//#include "DxWicTextureFactory.h"
//#include "Effects.h"
//#include "ModelData.h"
//#include "MyDxHelpers.h"
//#include "PrimitiveBatch.h"
#include "RotationGroup.h"
#include "SimpleMath.h"

#include "StepTimer.h"
#include "Core12.h"
//#include "vertexFactory.h"
//#include "VertexTypes.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace DXF
{
	class RendererDx12Imp : public Renderer
	{
	private:
		// Device resources.
		HWND m_window;
		int m_outputWidth;
		int m_outputHeight;

		// Rendering loop timer.
		DXF::StepTimer m_timer;

		RotationGroup m_rotationGroup;

		float m_nearPlaneView = 0.1f;
		float m_farPlaneView = 10.0f;

		Vertex<float> m_camera = GetDefaultCamera();
		Vertex<float> m_target = GetDefaultTarget();
		Vertex<float> m_targetBackground = GetDefaultTarget();

		DirectX::SimpleMath::Color m_backgroundColor;

		// new colors and lights
		DxEffectColors m_effectColors;
		DxLights m_lights;

		Vertex<float> m_worldScale = GetDefaultWorldScale();
		Vertex<float> m_backgroundScale = GetDefaultWorldScale();
		std::shared_ptr<DxSupport::Core12> m_core12;

		bool m_ready = false;

	public:

		RendererDx12Imp() :
			m_window(nullptr),
			m_outputWidth(400),
			m_outputHeight(400),
			m_backgroundColor(DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f)),	// r,g.b,a
			m_core12(new DxSupport::Core12())
		{}

		void Initialize(HWND window, int width, int height) override
		{
			m_window = window;
			m_outputWidth = std::max(width, 1);
			m_outputHeight = std::max(height, 1);

		}

		void Tick() override
		{
			m_timer.Tick([&]()
				{
//					Update(m_timer);
				});

			// Don't try to render anything before the first Update.
			if (m_timer.GetFrameCount() == 0)
				return;

//			Render();
		}

		void OnWindowSizeChanged(int width, int height) override
		{
			if (!IsReady())
				return;

			m_outputWidth = std::max(width, 1);
			m_outputHeight = std::max(height, 1);

			try
			{
//				CreateResources();
				m_ready = true;
			}
			catch (DxException& ex)
			{
				m_ready = false;
				CString msg(_T("Resource initialization failed! : "));
				msg += ex.what();
				AfxMessageBox(msg, MB_OK);
			}
		}

		void GetWindowSize(int& width, int& height) const override
		{
			width = m_outputWidth;
			height = m_outputHeight;
		}

		void SetModel(const DxVertexData& /*vertexData*/) override
		{

		}

		void SetModel2(const DxBackgroundVertexData& /*bkgndVertexData*/) override
		{

		}



		void SetRotationGroup(const RotationGroup& rg) override
		{
			m_rotationGroup = rg;
		}

		RotationGroup GetRotationGroup() override
		{
			return m_rotationGroup;
		}

		void RefreshRender(float /*time*/) override
		{

		}

		void SetTextureColors(std::vector<uint32_t> colors) override
		{

		}

		void SetTexture2()
		{

		}

		DxPerspective GetPerspective() const override
		{
			DxPerspective perspective;
			perspective.NearPlane = m_nearPlaneView;
			perspective.FarPlane = m_farPlaneView;

			return perspective;
		}

		void SetPerspective(const DxPerspective& perspective) override
		{
	//		if (!m_d3dDevice)
	//			return;

			float nearView = perspective.NearPlane;
			float farView = perspective.FarPlane;

			if (nearView > farView)
				std::swap(nearView, farView);

			m_nearPlaneView = nearView;
			m_farPlaneView = farView;

	//		CreateProjectionMatrix();
		}

		void SetView(const Vertex<float>& /*camera*/, const Vertex<float>& /*target*/, const Vertex<float>& /*targetBackgnd*/) override
		{

		}

		void SetTarget(const Vertex<float>& /*target*/) override
		{

		}

		void SetTargetBackground(const Vertex<float>& /*targetBackgnd*/) override
		{

		}

		Vertex<float> GetCamera() const override
		{
			return m_camera;
		}

		Vertex<float> GetTarget() const override
		{
			return m_target;
		}

		Vertex<float> GetTargetBackground() const override
		{
			return m_targetBackground;
		}

		bool IsReady() override
		{
			return false;
		}

		bool DrawImage(CDC& /*dc*/, CSize /*targetSize*/) override
		{
			return false;
		}

		CSize GetScreenSize() const override
		{
			return CSize(m_outputWidth, m_outputHeight);
		}

		void SetBackgroundColor(DirectX::SimpleMath::Color /*bkColor*/) override
		{

		}

		void SetEffectColors(DxEffectColors /*effectColors*/) override
		{

		}

		void SetLights(DxLights /*lights*/) override
		{

		}

		std::optional<DXF::Vertex<float>> Map2Dto3D(int /*x*/, int /*y*/) override
		{
			return std::nullopt;
		}

		void SetWorldScale(const DXF::Vertex<float>& scale) override
		{
			m_worldScale = scale;
		}

		Vertex<float> GetWorldScale() const override
		{
			return m_worldScale;
		}

		void SetBackgroundScale(const DXF::Vertex<float>& scale) override
		{
			m_backgroundScale = scale;
		}

		Vertex<float> GetBackgroundScale() const override
		{
			return m_backgroundScale;
		}

	protected:


		void OnDeviceLost() override
		{

		}






	};

	std::shared_ptr<Renderer> CreateRendererDx12()
	{
		return std::make_shared<RendererDx12Imp>();
	}
}