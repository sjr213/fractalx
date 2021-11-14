#include "stdafx.h"
#include "Core12.h"

#include <afxwin.h>
#include "NarrowCast.h"
#include "Common/GeometryGenerator.h"
#include "RotationParams.h"

using namespace DirectX;
using namespace DXF;
using namespace Microsoft::WRL;

namespace DxSupport
{

	namespace
	{
		std::vector<UINT8> GenerateTextureData(const std::vector<uint32_t>& colors)
		{
			std::vector<UINT8> components;
			components.reserve(4 * colors.size());

			for (const auto& color : colors)
			{
				components.push_back((uint8_t)(color >> 24));
				components.push_back((uint8_t)(color >> 16));
				components.push_back((uint8_t)(color >> 8));
				components.push_back((uint8_t)color);
			}

			return components;
		}

		static float RoundDegrees(float deg)
		{
			return fmod(deg, 360.0f);
		}
	}

Core12::Core12()
{}

Core12::~Core12()
{}

void Core12::SetVertices(std::vector<Vertex>&& vertices)
{
	m_vertices = vertices;
}

void Core12::SetIndices(std::vector<std::int32_t>&& indices)
{
	m_indices = indices;
}

void Core12::SetTextureColors(std::vector<uint32_t> colors)
{
	m_colors = colors;
}

DXF::Vertex<float> Core12::GetCamera() const
{
	return m_camera;
}

void Core12::SetCamera(const DXF::Vertex<float>& camera)
{
	m_camera = camera;
}

DXF::Vertex<float> Core12::GetTarget() const
{
	return m_target;
}

void Core12::SetTarget(const DXF::Vertex<float>& target)
{
	m_target = target;
}

void Core12::SetPerspective(float nearView, float farView)
{
	m_near = nearView;
	m_far = farView;

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), m_near, m_far);
	XMStoreFloat4x4(&m_proj, P);
}

float Core12::GetNearPlane() const
{
	return m_near;
}

float Core12::GetFarPlane() const
{
	return m_far;
}

void Core12::SetRotationGroup(const RotationGroup& rg)
{
	m_rotationGroup = rg;
}

RotationGroup Core12::GetRotationGroup() const
{
	return m_rotationGroup;
}

void Core12::SetBackgroundColor(const DirectX::XMFLOAT4& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

void Core12::SetAmbientColor(const DirectX::XMFLOAT4& ambientColor)
{
	m_ambientLight = ambientColor;
}

void Core12::SetDiffuseAlbedo(const DirectX::XMFLOAT4& diffuseAlbedo)
{
	m_diffuseAlbedo = diffuseAlbedo;
}

void Core12::SetFresnelR0(const DirectX::XMFLOAT3& fresnelR0)
{
	m_fresnelR0 = fresnelR0;
}

void Core12::SetRoughness(float roughness)
{
	m_roughness = roughness;
}

void Core12::SetLight(int index, DirectX::XMFLOAT3& color, DirectX::XMFLOAT3& direction)
{
	if (index < 0 || index > 2)
	{
		assert(false);
		return;
	}

	m_lights[index].Strength = color;
	m_lights[index].Direction = direction;
}

void Core12::SetWorldScale(const DXF::Vertex<float>& scale) 
{
	m_worldScale = scale;
}

DXF::Vertex<float> Core12::GetWorldScale() const
{
	return m_worldScale;
}

void Core12::SetBackgroundScale(const DXF::Vertex<float>& scale)
{
	m_backgroundScale = scale;
}

DXF::Vertex<float> Core12::GetWBackgroundScale() const
{
	return m_backgroundScale;
}

bool Core12::Initialize(HWND mainWnd, int width, int height)
{
	if (!Core12Base::Initialize(mainWnd, width, height)) 
		return false;

	return BuildAll();
}

bool Core12::BuildAll()
{
	if (MainWnd() == NULL)
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr));

	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	m_cbvSrvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // ok

	LoadTextures();
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildMainModelGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	// Execute the initialization commands.
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void Core12::Resize(int width, int height)
{
	Core12Base::Resize(width, height);

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), m_near, m_far);
	XMStoreFloat4x4(&m_proj, P);
}

bool Core12::IsReadyToRender() // Need to think about this
{
	return m_mainRitem != nullptr;
}

void Core12::Update(float totalTimeSecs, float deltaTimeSecs)
{
	SetWorlds(totalTimeSecs);
	UpdateCamera();

	// Cycle through the circular frame resource array.
	m_currFrameResourceIndex = (m_currFrameResourceIndex + 1) % g_NumFrameResources;
	m_currFrameResource = m_frameResources[m_currFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (m_currFrameResource->Fence != 0 && m_fence->GetCompletedValue() < m_currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials();
	UpdateObjectCBs();
	UpdateMaterialCBs();
	UpdateMainPassCB(totalTimeSecs, deltaTimeSecs);
	UpdateReflectedPassCB();
}

void Core12::Draw()
{
	auto cmdListAlloc = m_currFrameResource->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_commandList->Reset(cmdListAlloc.Get(), m_PSOs["opaque"].Get()));

	m_commandList->RSSetViewports(1, &m_screenViewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Indicate a state transition on the resource usage.
	auto backBuffer = CurrentBackBuffer();
	auto barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrierToRenderTarget);

	// Clear the back buffer and depth buffer.
	m_commandList->ClearRenderTargetView(CurrentBackBufferView(), (float*)&m_mainPassCB.FogColor, 0, nullptr);
	m_commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	auto backBufferDesc = CurrentBackBufferView();
	auto depthStencilDesc = DepthStencilView();
	m_commandList->OMSetRenderTargets(1, &backBufferDesc, true, &depthStencilDesc);

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvDescriptorHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

//	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

	// Draw opaque items--floors, walls, skull.
	auto passCB = m_currFrameResource->PassCB->Resource();
	m_commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	DrawRenderItems(m_commandList.Get(), m_ritemLayer[(int)RenderLayer::Opaque]);

	// Indicate a state transition on the resource usage.
	auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrierToPresent);

	// Done recording commands.
	ThrowIfFailed(m_commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currBackBuffer = (m_currBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	m_currFrameResource->Fence = ++m_currentFence;

	// Notify the fence when the GPU completes commands up to this fence point.
	m_commandQueue->Signal(m_fence.Get(), m_currentFence);
}

void Core12::SetWorlds(float totalTime)
{
	float radians = totalTime / 2;
	float degrees = XMConvertToDegrees(radians);
	auto transMatrix = XMMatrixTranslation(m_target.X, m_target.Y, m_target.Z);

	if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateX)
	{
		m_rotationGroup.RotationParamsMain.AngleXDegrees = RoundDegrees(degrees);
	}
	else if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateY)
	{
		m_rotationGroup.RotationParamsMain.AngleYDegrees = RoundDegrees(degrees);
	}
	else if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateZ)
	{
		m_rotationGroup.RotationParamsMain.AngleZDegrees = RoundDegrees(degrees);
	}
	else if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateXY)
	{
		m_rotationGroup.RotationParamsMain.AngleXDegrees = RoundDegrees(degrees);
		m_rotationGroup.RotationParamsMain.AngleYDegrees = RoundDegrees(degrees);
	}
	else if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateYZ)
	{
		m_rotationGroup.RotationParamsMain.AngleYDegrees = RoundDegrees(degrees);
		m_rotationGroup.RotationParamsMain.AngleZDegrees = RoundDegrees(degrees);
	}
	else if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateXZ)
	{
		m_rotationGroup.RotationParamsMain.AngleXDegrees = RoundDegrees(degrees);
		m_rotationGroup.RotationParamsMain.AngleZDegrees = RoundDegrees(degrees);
	}
	else if (m_rotationGroup.RotationParamsMain.Action == RotationAction::RotateAll)
	{
		m_rotationGroup.RotationParamsMain.AngleYDegrees = RoundDegrees(degrees);
		m_rotationGroup.RotationParamsMain.AngleZDegrees = RoundDegrees(degrees);
		m_rotationGroup.RotationParamsMain.AngleXDegrees = RoundDegrees(degrees);
	}

	XMMATRIX mainScale = XMMatrixScaling(m_worldScale.X, m_worldScale.Y, m_worldScale.Z);
	XMMATRIX rotateY = XMMatrixRotationY(XMConvertToRadians(m_rotationGroup.RotationParamsMain.AngleYDegrees));
	XMMATRIX rotateX = XMMatrixRotationX(XMConvertToRadians(m_rotationGroup.RotationParamsMain.AngleXDegrees));
	XMMATRIX rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_rotationGroup.RotationParamsMain.AngleZDegrees));
	XMMATRIX mainWorld = mainScale * rotateY * rotateX * rotateZ * transMatrix;
	XMStoreFloat4x4(&m_mainRitem->World, mainWorld);

	m_mainRitem->NumFramesDirty = g_NumFrameResources;
}

void Core12::UpdateCamera()
{
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_camera.X, m_camera.Y, m_camera.Z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_view, view);
}

void Core12::AnimateMaterials()
{
}

void Core12::UpdateObjectCBs()
{
	auto currObjectCB = m_currFrameResource->ObjectCB.get();
	for (auto& e : m_allRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void Core12::UpdateMaterialCBs()
{
	auto currMaterialCB = m_currFrameResource->MaterialCB.get();
	for (auto& e : m_materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void Core12::UpdateMainPassCB(float totalTimeSecs, float deltaTimeSecs)
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	auto viewDet = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&viewDet, view);
	auto projDet = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&projDet, proj);
	auto viewProjDet = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&viewProjDet, viewProj);

	XMStoreFloat4x4(&m_mainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_mainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&m_mainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&m_mainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_mainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&m_mainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	m_mainPassCB.EyePosW = m_eyePos;
	m_mainPassCB.RenderTargetSize = XMFLOAT2((float)m_clientWidth, (float)m_clientHeight);
	m_mainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_clientWidth, 1.0f / m_clientHeight);
	m_mainPassCB.NearZ = 1.0f;
	m_mainPassCB.FarZ = 1000.0f;
	m_mainPassCB.TotalTime = totalTimeSecs;
	m_mainPassCB.DeltaTime = deltaTimeSecs;
	m_mainPassCB.AmbientLight = m_ambientLight;
	m_mainPassCB.Lights[0] = m_lights[0];
	m_mainPassCB.Lights[1] = m_lights[1];
	m_mainPassCB.Lights[2] = m_lights[2];
	m_mainPassCB.FogColor = m_backgroundColor;

	// Main pass stored in index 2
	auto currPassCB = m_currFrameResource->PassCB.get();
	currPassCB->CopyData(0, m_mainPassCB);
}

void Core12::UpdateReflectedPassCB()
{
	m_reflectedPassCB = m_mainPassCB;

	XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	XMMATRIX R = XMMatrixReflect(mirrorPlane);

	// Reflect the lighting.
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR lightDir = XMLoadFloat3(&m_mainPassCB.Lights[i].Direction);
		XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&m_reflectedPassCB.Lights[i].Direction, reflectedLightDir);
	}

	// Reflected pass stored in index 1
	auto currPassCB = m_currFrameResource->PassCB.get();
	currPassCB->CopyData(1, m_reflectedPassCB);
}

void Core12::ReloadTextures()
{
	if (m_d3dDevice == nullptr)
		return;

	// the textures don't exist yet they will be created later during intialization
	// otherwise continue to replace the textures
	if (m_textures.empty())
		return;

	// Cycle through the circular frame resource array.
	m_currFrameResourceIndex = (m_currFrameResourceIndex + 1) % g_NumFrameResources;
	m_currFrameResource = m_frameResources[m_currFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (m_currFrameResource->Fence != 0 && m_fence->GetCompletedValue() < m_currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	auto cmdListAlloc = m_currFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(m_commandList->Reset(cmdListAlloc.Get(), nullptr));

	LoadTextures();

	// Execute the initialization commands.
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void Core12::LoadTextures()
{
	// from https://stackoverflow.com/questions/35568302/what-is-the-d3d12-equivalent-of-d3d11-createtexture2d

	std::string textureName = "colorTex";
	m_textures[textureName].reset();
	auto colorTex = std::make_unique<Texture>();
	colorTex->Name = textureName;

	// Create the texture.
	UINT textureWidth = DxCore::narrow<UINT>(m_colors.size());
	UINT textureHeight = 1;

	// Describe and create a Texture2D.
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// each element is [0,1]
	textureDesc.Width = textureWidth;				
	textureDesc.Height = textureHeight;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	auto heapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapPropDefault,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(colorTex->Resource.ReleaseAndGetAddressOf())));

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(colorTex->Resource.Get(), 0, 1);

	// Create the GPU upload buffer.
	auto heapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDescr = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapPropUpload,
		D3D12_HEAP_FLAG_NONE,
		&resourceDescr,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(colorTex->UploadHeap.ReleaseAndGetAddressOf())));

	// Copy data to the intermediate upload heap and then schedule a copy 
	// from the upload heap to the Texture2D.
	std::vector<UINT8> texture = GenerateTextureData(m_colors);

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &texture[0];
	textureData.RowPitch = textureWidth * sizeof(uint8_t);				// not sure about this
	textureData.SlicePitch = textureData.RowPitch * textureHeight;

	UpdateSubresources(m_commandList.Get(), colorTex->Resource.Get(), colorTex->UploadHeap.Get(), 0, 0, 1, &textureData);
	auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(colorTex->Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_commandList->ResourceBarrier(1, &resourceBarrier);
	
	m_textures[colorTex->Name] = std::move(colorTex);
}

void Core12::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Performance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);
	slotRootParameter[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_d3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
}

void Core12::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvDescriptorHeap)));

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto colorTex = m_textures["colorTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = colorTex->GetDesc().Format;						// might need to hard code this to DXGI_FORMAT_R8G8B8A8_UNORM
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = DxCore::narrow_cast<UINT>(-1);
	m_d3dDevice->CreateShaderResourceView(colorTex.Get(), &srvDesc, hDescriptor);
}

void Core12::BuildShadersAndInputLayout()
{

	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};
/*
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};
*/
	m_shaders["standardVS"] = d3dUtil::CompileShader(L"..\\DxSupport\\Shaders\\Default1dTexture.hlsl", nullptr, "VS", "vs_5_0");
	m_shaders["opaquePS"] = d3dUtil::CompileShader(L"..\\DxSupport\\Shaders\\Default1dTexture.hlsl", defines, "PS", "ps_5_0");
//	m_shaders["alphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\DefaultStencil.hlsl", alphaTestDefines, "PS", "ps_5_0");

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void Core12::BuildMainModelGeometry()
{
	// new 
	SubmeshGeometry mainSubmesh;
	mainSubmesh.IndexCount = DxCore::narrow<UINT>(m_indices.size());
	mainSubmesh.StartIndexLocation = 0;
	mainSubmesh.BaseVertexLocation = 0;

	const UINT vbByteSize = DxCore::narrow<UINT>(m_vertices.size() * sizeof(Vertex));
	const UINT ibByteSize = DxCore::narrow<UINT>(m_indices.size() * sizeof(std::uint32_t));

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "mainModel";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), m_vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), m_indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_commandList.Get(), m_vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_commandList.Get(), m_indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["mainModel"] = mainSubmesh;

	m_geometries[geo->Name] = std::move(geo);
}


void Core12::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	opaquePsoDesc.pRootSignature = m_rootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shaders["standardVS"]->GetBufferPointer()),
		m_shaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shaders["opaquePS"]->GetBufferPointer()),
		m_shaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_backBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = m_depthStencilFormat;
	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["opaque"])));
}

void Core12::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		m_frameResources.push_back(std::make_unique<FrameResourceFx>(m_d3dDevice.Get(),
			2, (UINT)m_allRitems.size(), (UINT)m_materials.size()));
	}
}

void Core12::BuildMaterials()
{
	auto defaultMat = std::make_unique<Material>();
	defaultMat->Name = "default";
	defaultMat->MatCBIndex = 0;
	defaultMat->DiffuseSrvHeapIndex = 0;
	defaultMat->DiffuseAlbedo = m_diffuseAlbedo;
	defaultMat->FresnelR0 = m_fresnelR0;
	defaultMat->Roughness = m_roughness;

	m_materials["default"] = std::move(defaultMat);
}

void Core12::BuildRenderItems()
{
	auto mainModelRitem = std::make_unique<RenderItem>();
	mainModelRitem->World = MathHelper::Identity4x4();
	mainModelRitem->TexTransform = MathHelper::Identity4x4();
	mainModelRitem->ObjCBIndex = 0;
	mainModelRitem->Mat = m_materials["default"].get();						// changed
	mainModelRitem->Geo = m_geometries["mainModel"].get();
	mainModelRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mainModelRitem->IndexCount = mainModelRitem->Geo->DrawArgs["mainModel"].IndexCount;
	mainModelRitem->StartIndexLocation = mainModelRitem->Geo->DrawArgs["mainModel"].StartIndexLocation;
	mainModelRitem->BaseVertexLocation = mainModelRitem->Geo->DrawArgs["mainModel"].BaseVertexLocation;
	m_mainRitem = mainModelRitem.get();
	m_ritemLayer[(int)RenderLayer::Opaque].clear();
	m_ritemLayer[(int)RenderLayer::Opaque].push_back(mainModelRitem.get());

	m_allRitems.push_back(std::move(mainModelRitem));
}

void Core12::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_currFrameResource->ObjectCB->Resource();
	auto matCB = m_currFrameResource->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		auto vertexBufferView = ri->Geo->VertexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
		auto indexBufferView = ri->Geo->IndexBufferView();
		cmdList->IASetIndexBuffer(&indexBufferView);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, m_cbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Core12::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

bool Core12::DrawImage(CDC& dc, CSize targetSize)
{
	if (!IsReadyToRender())
		return false;

	ComPtr<IDXGISurface1> pSurface1;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)pSurface1.GetAddressOf());
	if (FAILED(hr))
		return false;

	HDC hdc3D;
	pSurface1->GetDC(FALSE, &hdc3D);

	HBITMAP hbm3D = static_cast<HBITMAP>(GetCurrentObject(hdc3D, OBJ_BITMAP));

	BITMAPINFO	bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biBitCount = 0;

	VERIFY(GetDIBits(hdc3D, hbm3D, 0, bitmapInfo.bmiHeader.biHeight, nullptr, &bitmapInfo, DIB_RGB_COLORS));

	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	auto nBytes = static_cast<unsigned long long>(bitmapInfo.bmiHeader.biWidth + 1);
	nBytes *= static_cast<unsigned long long>(bitmapInfo.bmiHeader.biHeight);
	nBytes *= sizeof(DWORD);
	std::unique_ptr<BYTE[]> pDIBits(new BYTE[nBytes]);
	VERIFY(GetDIBits(hdc3D, hbm3D, 0, bitmapInfo.bmiHeader.biHeight, pDIBits.get(), &bitmapInfo, DIB_RGB_COLORS));

	// Better quality stretching
	int oldMode = SetStretchBltMode(dc, HALFTONE);

	VERIFY(StretchDIBits(dc, 0, 0, targetSize.cx, targetSize.cy, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight,
		pDIBits.get(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY) != GDI_ERROR);

	pSurface1->ReleaseDC(nullptr);
	SetStretchBltMode(dc, oldMode);

	return true;
}

} // namespace DxSupport