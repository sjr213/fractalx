#pragma once

// Copy and adapt from StencilDemoApp.cpp

#include "Core12Base.h"
#include <DirectXMath.h>
#include "FrameResourceStencil.h"
#include "GameTimer.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

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

class Core12Stencil : public Core12Base
{
public:
	Core12Stencil();
	Core12Stencil(const Core12Stencil& rhs) = delete;
	Core12Stencil& operator=(const Core12Stencil& rhs) = delete;
	~Core12Stencil();

	bool Initialize(HWND mainWnd, int width, int height) override;

	void OnTimer(bool start);

	void Resize(int width, int height) override;
	void Update() override;
	void Update(const GameTimer& gt);
	void Draw() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera();
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs();
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);					
	void UpdateReflectedPassCB(const GameTimer& gt);

	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildRoomGeometry();
	void BuildSkullGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:

	std::vector<std::unique_ptr<FrameResourceStencil>> m_frameResources;
	FrameResourceStencil* m_currFrameResource = nullptr;
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
	RenderItem* m_skullRitem = nullptr;
	RenderItem* m_reflectedSkullRitem = nullptr;
	RenderItem* m_shadowedSkullRitem = nullptr;

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

	POINT m_lastMousePos;
	GameTimer m_timer;
};