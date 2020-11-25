#pragma once

#include "Core12Base.h"
#include <DirectXMath.h>
#include "MathHelper.h"
#include "UploadBuffer.h"

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class Core12Box : public Core12Base
{
public:
	Core12Box();
	Core12Box(const Core12Box& rhs) = delete;
	Core12Box& operator=(const Core12Box& rhs) = delete;
	~Core12Box();

	bool Initialize(HWND mainWnd, int width, int height) override;
	void Resize(int width, int height) override;
	void Update() override;
	void Draw() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:
	
	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();

private:

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_objectCB = nullptr;

	std::unique_ptr<MeshGeometry> m_boxGeo = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO = nullptr;

	DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();

	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV4;
	float m_radius = 5.0f;

	// do we need this?
	POINT m_lastMousePos;
};