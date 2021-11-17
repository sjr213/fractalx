
#include "pch.h"
#include "Renderer.h"

#include <afxwin.h>
#include "ColorUtil.h"
#include "Core12.h"
#include <DirectXColors.h>
#include "Dx12CoordinateMap.h"
#include "DxEffectColors.h"
#include "DxException.h"
#include "DxLight.h"
#include <dxgi.h>
#include "FrameResourceFx.h"
#include "NarrowCast.h"
#include "RotationGroup.h"
#include "SimpleMath.h"
#include "StepTimer.h"
#include <wrl/client.h>

using namespace DxCore;
using namespace DxSupport;
using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace DXF
{
	class RendererDx12Imp : public Renderer
	{
	private:
		// Rendering loop timer.
		DXF::StepTimer m_timer;

		Vertex<float> m_targetBackground = GetDefaultTarget();

		std::unique_ptr<DxSupport::Core12> m_core12;

	public:

		RendererDx12Imp() :
			m_core12(new DxSupport::Core12())
		{}

		virtual ~RendererDx12Imp()
		{}

		void Initialize(HWND window, int width, int height) override
		{
			m_core12->Initialize(window, width, height);
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

			if (!m_core12->IsReadyToRender())
				return;

			auto size = m_core12->GetClientSize();

			if (width == size.cx && height == size.cy)
				return;

			try
			{
				m_core12->Resize(width, height);
				m_core12->Update(0, 0);
				m_core12->Draw();
			}
			catch (DxException& ex)
			{
				CString msg(_T("Resource initialization failed! : "));
				msg += ex.what();
				AfxMessageBox(msg, MB_OK);
			}
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
			m_core12->SetRotationGroup(Dx12CoordinateMap::ToD12RotationGroup(rg));
		}

		RotationGroup GetRotationGroup() override
		{
			return Dx12CoordinateMap::FromD12RotationGroup(m_core12->GetRotationGroup());
		}

		void RefreshRender(float time) override
		{
			if (!m_core12->IsReadyToRender())
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
		}

		void SetTexture2()
		{

		}

		DxPerspective GetPerspective() const override
		{
			DxPerspective perspective;
			perspective.NearPlane = m_core12->GetNearPlane();
			perspective.FarPlane = m_core12->GetFarPlane();

			return perspective;
		}

		void SetPerspective(const DxPerspective& perspective) override
		{
			float nearView = perspective.NearPlane;
			float farView = perspective.FarPlane;

			if (nearView > farView)
				std::swap(nearView, farView);

			m_core12->SetPerspective(nearView, farView);
		}

		void SetView(const Vertex<float>& camera, const Vertex<float>& target, const Vertex<float>& /*targetBackgnd*/) override
		{
			// Since dx12 uses a LH lookat matrix and dx11 uses a RH lookat
			// we use the negative x value to Dx12 looks like Dx11
			auto cameraCpy = camera;
			cameraCpy.X *= -1.0f;
			m_core12->SetCamera(cameraCpy);
			m_core12->SetTarget(Dx12CoordinateMap::ToD12Target(target));
		}

		void SetTarget(const Vertex<float>& target) override
		{
			m_core12->SetTarget(Dx12CoordinateMap::ToD12Target(target));
		}

		void SetTargetBackground(const Vertex<float>& /*targetBackgnd*/) override
		{
			// Dx12CoordinateMap::ToD12Target(target)
		}

		Vertex<float> GetCamera() const override
		{
			// Since dx12 uses a LH lookat matrix and dx11 uses a RH lookat
			// we use the negative x value to Dx12 looks like Dx11
			auto camera = m_core12->GetCamera();
			camera.X *= -1.0f;
			return camera;
		}

		Vertex<float> GetTarget() const override
		{
			return Dx12CoordinateMap::FromD12Target(m_core12->GetTarget());
		}

		Vertex<float> GetTargetBackground() const override
		{
			// Dx12CoordinateMap::FromD12Target(m_core12->GetTarget());
			return m_targetBackground;
		}

		bool IsReady() override
		{
			return m_core12 != nullptr;  
		}

		bool DrawImage(CDC& dc, CSize targetSize) override
		{
			return m_core12->DrawImage(dc, targetSize);
		}

		CSize GetScreenSize() const override
		{
			auto size = m_core12->GetClientSize();
			return CSize(size.cx, size.cy);
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
			m_core12->SetRoughness(effectColors.Roughness);
			m_core12->BuildAll();
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

	std::unique_ptr<Renderer> CreateRendererDx12()
	{
		return std::make_unique<RendererDx12Imp>();
	}
}