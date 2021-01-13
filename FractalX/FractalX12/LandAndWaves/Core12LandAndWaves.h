#pragma once

// Copy and adapt from LandAndWavesApp.cpp

#include "Core12Base.h"
#include <DirectXMath.h>
#include "FrameResourceLandAndWaves.h"
#include "GameTimer.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "Waves.h"



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
	UINT ObjCBIndex = -1;

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
	Count
};

class Core12LandAndWaves : public Core12Base
{
public:
	Core12LandAndWaves();
	Core12LandAndWaves(const Core12LandAndWaves& rhs) = delete;
	Core12LandAndWaves& operator=(const Core12LandAndWaves& rhs) = delete;
	~Core12LandAndWaves();

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
	void UpdateObjectCBs();
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);						// <---- NEED TIME
	void UpdateWaves(const GameTimer& gt);								// <---- NEED TIME

	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildLandGeometry();
	void BuildWavesGeometryBuffers();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	float GetHillsHeight(float x, float z)const;
	DirectX::XMFLOAT3 GetHillsNormal(float x, float z)const;

private:

	std::vector<std::unique_ptr<FrameResourceLandAndWaves>> m_frameResources;
	FrameResourceLandAndWaves* m_currFrameResource = nullptr;
	int m_currFrameResourceIndex = 0;

	UINT m_cbvSrvDescriptorSize = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	RenderItem* m_wavesRitem = nullptr;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> m_allRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> m_ritemLayer[(int)RenderLayer::Count];

	std::unique_ptr<Waves> m_waves;

	PassConstants m_mainPassCB;

	bool m_isWireframe = false;		// Might not need

	DirectX::XMFLOAT3 m_eyePos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();

	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV2 - 0.1f;
	float m_radius = 50.0f;

	float m_sunTheta = 1.25f * DirectX::XM_PI;
	float m_sunPhi = DirectX::XM_PIDIV4;

	POINT m_lastMousePos;
	GameTimer m_timer;
};