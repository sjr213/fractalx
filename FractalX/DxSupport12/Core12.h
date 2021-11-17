#pragma once

// Copy and adapt from StencilDemoApp.cpp

#include "Core12Base.h"
#include "DefaultFields.h"
#include <DirectXMath.h>
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "RotationGroup.h"
#include "FrameResourceFx.h"
#include "Vertex.h"

class CDC;

namespace DxSupport
{
	// was gNumFrameResources
	const int g_NumFrameResources = 3;

	// Lightweight structure stores parameters to draw a shape.  This will
	// vary from app-to-app.
	struct RenderItem
	{
		RenderItem() = default;

		// World matrix of the shape that describes the object's local space
		// relative to the world space, which defines the position, orientation,
		// and scale of the object in the world.
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

		// Dirty flag indicating the object data has changed and we need to update the constant buffer.
		// Because we have an object cbuffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify obect data we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		int NumFramesDirty = g_NumFrameResources;

		// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
		UINT ObjCBIndex = 0;

		Material* Mat = nullptr;
		MeshGeometry* Geo = nullptr;

		// Primitive topology.
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// DrawIndexedInstanced parameters.
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	enum class RenderLayer : int
	{
		Opaque = 0,
		Mirrors,
		Reflected,
		Transparent,
		Shadow,
		Count
	};

	class Core12 : public Core12Base
	{
	public:
		Core12();
		Core12(const Core12& rhs) = delete;
		Core12& operator=(const Core12& rhs) = delete;
		~Core12();

		bool Initialize(HWND mainWnd, int width, int height) override;

		void Resize(int width, int height) override;
		void Update(float totalTimeSecs, float deltaTimeSecs);
		void Draw() override;

		// For FractalX
		void SetVertices(std::vector<Vertex>&& vertices);
		void SetIndices(std::vector<std::int32_t>&& indices);
		void SetTextureColors(std::vector<uint32_t> colors);

		DXF::Vertex<float> GetCamera() const;
		void SetCamera(const DXF::Vertex<float>& camera);
		DXF::Vertex<float> GetTarget() const;
		void SetTarget(const DXF::Vertex<float>& target);
		void SetPerspective(float nearView, float farView);

		void SetRotationGroup(const DXF::RotationGroup& rg);
		DXF::RotationGroup GetRotationGroup() const;

		void SetBackgroundColor(const DirectX::XMFLOAT4& backgroundColor);
		void SetAmbientColor(const DirectX::XMFLOAT4& ambientColor);
		void SetDiffuseAlbedo(const DirectX::XMFLOAT4& diffuseAlbedo);
		void SetFresnelR0(const DirectX::XMFLOAT3& fresnelR0);
		void SetRoughness(float roughness);
		void SetLight(int index, DirectX::XMFLOAT3& color, DirectX::XMFLOAT3& direction);
		void SetWorldScale(const DXF::Vertex<float>& scale);
		DXF::Vertex<float> GetWorldScale() const;
		void SetBackgroundScale(const DXF::Vertex<float>& scale);
		DXF::Vertex<float> GetWBackgroundScale() const;
		void SetWorlds(float totalTime); // replaces OnKeyboardInput()

		bool IsReadyToRender();

		void ReloadTextures();

		float GetNearPlane() const;
		float GetFarPlane() const;

		bool BuildAll();

		bool DrawImage(CDC& dc, CSize targetSize);

	private:
	
		void UpdateCamera();
		void AnimateMaterials();
		void UpdateObjectCBs();
		void UpdateMaterialCBs();
		void UpdateMainPassCB(float totalTimeSecs, float deltaTimeSecs);
		void UpdateReflectedPassCB();

		void LoadTextures();
		void BuildRootSignature();
		void BuildDescriptorHeaps();
		void BuildShadersAndInputLayout();
		void BuildMainModelGeometry();
		void BuildPSOs();
		void BuildFrameResources();
		void BuildMaterials();
		void BuildRenderItems();
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	private:

		std::vector<std::unique_ptr<FrameResourceFx>> m_frameResources;
		FrameResourceFx* m_currFrameResource = nullptr;
		int m_currFrameResourceIndex = 0;

		UINT m_cbvSrvDescriptorSize = 0;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvDescriptorHeap = nullptr;

		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PSOs;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

		// Cache render items of interest.
		RenderItem* m_mainRitem = nullptr;

		// List of all the render items.
		std::vector<std::unique_ptr<RenderItem>> m_allRitems;

		// Render items divided by PSO.
		std::vector<RenderItem*> m_ritemLayer[(int)RenderLayer::Count];

		PassConstants m_mainPassCB;
		PassConstants m_reflectedPassCB;

		DirectX::XMFLOAT3 m_skullTranslation = { 0.0f, 1.0f, -5.0f };

		DirectX::XMFLOAT3 m_eyePos = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();

		float m_theta = 1.5f * DirectX::XM_PI;
		float m_phi = 0.42f * DirectX::XM_PI;
		float m_radius = 12.0f;

		// new for FractalX
		std::vector<Vertex> m_vertices;
		std::vector<std::int32_t> m_indices;
		std::vector<uint32_t> m_colors;
		DXF::Vertex<float> m_camera = DXF::GetDefaultCamera();
		DXF::Vertex<float> m_target = DXF::GetDefaultTarget();
		float m_near = 1.0f;
		float m_far = 1000.0f;
		DXF::RotationGroup m_rotationGroup;
		DirectX::XMFLOAT4 m_backgroundColor = {1.0f, 0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT4 m_ambientLight = { 0.25f, 0.25f, 0.25f, 1.0f };
		DirectX::XMFLOAT4 m_diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 m_fresnelR0 = { 0.05f, 0.05f, 0.05f };
		float m_roughness = 0.125;
		Light m_lights[3];
		DXF::Vertex<float> m_worldScale = DXF::GetDefaultWorldScale();
		DXF::Vertex<float> m_backgroundScale = DXF::GetDefaultWorldScale();
	};
}