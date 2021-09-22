
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
#include "FrameResourceFx.h"
#include "NarrowCast.h"
#include "ColorUtil.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace DxCore;

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

		float m_nearPlaneView = 0.1f;
		float m_farPlaneView = 10.0f;

		Vertex<float> m_camera = GetDefaultCamera();
		Vertex<float> m_target = GetDefaultTarget();
		Vertex<float> m_targetBackground = GetDefaultTarget();

		DirectX::SimpleMath::Color m_backgroundColor;

		// new colors and lights
		DxEffectColors m_effectColors;
		DxLights m_lights;

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
			m_window = window;							// maybe remove m_window
			m_outputWidth = std::max(width, 1);			// maybe remove m_outputWidth
			m_outputHeight = std::max(height, 1);		// maybe remove m_outputHeight
			m_core12->Initialize(m_window, m_outputWidth, m_outputHeight);
		}

		void Tick() override
		{
			m_timer.Tick([&]()
				{
					Update(m_timer);
				});

			// Don't try to render anything before the first Update.
			if (m_timer.GetFrameCount() == 0)
				return;

			m_core12->Draw();
		}

		void Update(DXF::StepTimer const& timer)
		{
			float time = float(timer.GetTotalSeconds());
			float deltaTime = float(timer.GetElapsedSeconds());

			m_core12->Update(time, deltaTime);
		}

		void OnWindowSizeChanged(int width, int height) override
		{
			if (!IsReady())
				return;

			if (width == m_outputWidth && height == m_outputHeight)
				return;

			m_outputWidth = std::max(width, 1);
			m_outputHeight = std::max(height, 1);

			try
			{
				m_core12->Resize(width, height);
				m_ready = true;
				m_core12->Update(0, 0);
				m_core12->Draw();
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

		void SetModel(const DxVertexData& vertexData) override
		{
			std::vector<DxSupport::Vertex> vertices;
			vertices.reserve(vertexData.Vertices.size());
			std::for_each(vertexData.Vertices.begin(), vertexData.Vertices.end(),
				[&vertices](const DirectX::VertexPositionNormalTexture& v)
				{
					vertices.emplace_back(v.position, v.normal, v.textureCoordinate);
				});
			m_core12->SetVertices(std::move(vertices));

			std::vector<int32_t> indices;
			indices.reserve(vertexData.Indices.size());
			std::for_each(vertexData.Indices.begin(), vertexData.Indices.end(),
				[&indices](const uint32_t& i)
				{
					indices.push_back(narrow<int32_t>(i));
				});
			m_core12->SetIndices(std::move(indices));
		}

		void SetModel2(const DxBackgroundVertexData& /*bkgndVertexData*/) override
		{

		}

		void SetRotationGroup(const RotationGroup& rg) override
		{
			m_core12->SetRotationGroup(rg);
		}

		RotationGroup GetRotationGroup() override
		{
			return m_core12->GetRotationGroup();
		}

		void RefreshRender(float time) override
		{
			if (!m_core12->IsReady())
				return;

			m_core12->Update(time, time);
			m_core12->Draw();
		}

		void SetTextureColors(std::vector<uint32_t> colors) override
		{
			// convert colors from argb to rgba
			m_core12->SetTextureColors(ColorUtil::ConvertFromARGBtoRGBA(colors));
		}

		void ReloadTextures() override
		{
			m_core12->ReloadTextures();
			m_core12->Update(0, 0);
			m_core12->Draw();
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
			float nearView = perspective.NearPlane;
			float farView = perspective.FarPlane;

			if (nearView > farView)
				std::swap(nearView, farView);

			m_nearPlaneView = nearView;
			m_farPlaneView = farView;

			m_core12->SetPerspective(nearView, farView);
		}

		void SetView(const Vertex<float>& camera, const Vertex<float>& target, const Vertex<float>& /*targetBackgnd*/) override
		{
			m_core12->SetCamera(camera);
			m_core12->SetTarget(target);
		}

		void SetTarget(const Vertex<float>& target) override
		{
			m_core12->SetTarget(target);
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
			return m_core12 != nullptr;
		}

		bool DrawImage(CDC& /*dc*/, CSize /*targetSize*/) override
		{
			return false;
		}

		CSize GetScreenSize() const override
		{
			return CSize(m_outputWidth, m_outputHeight);
		}

		void SetBackgroundColor(DirectX::SimpleMath::Color bkColor) override
		{
			m_core12->SetBackgroundColor(bkColor);
		}

		void SetEffectColors(DxEffectColors effectColors) override
		{
			// These values have different meanings than Dx11
			m_core12->SetAmbientColor(effectColors.AmbientColor);
			m_core12->SetDiffuseAlbedo(effectColors.DiffuseColor);
			m_core12->SetFresnelR0(effectColors.SpecularColor.ToVector3());
		}

		void SetLights(DxLights lights) override
		{
			for (int i = 0; i < 3; ++i)
			{
				auto color = lights.Lights.at(i).DiffuseColor.ToVector3();
				if (lights.Lights.at(i).Enable == false)
					color = { 0,0,0 };

				m_core12->SetLight(i, color, lights.Lights.at(i).Direction);
			}
		}

		std::optional<DXF::Vertex<float>> Map2Dto3D(int /*x*/, int /*y*/) override
		{
			return std::nullopt;
		}

		void SetWorldScale(const DXF::Vertex<float>& scale) override
		{
			m_core12->SetWorldScale(scale);
		}

		Vertex<float> GetWorldScale() const override
		{
			return m_core12->GetWorldScale();
		}

		void SetBackgroundScale(const DXF::Vertex<float>& scale) override
		{
			m_core12->SetBackgroundScale(scale);
		}

		Vertex<float> GetBackgroundScale() const override
		{
			return m_core12->GetWBackgroundScale();
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