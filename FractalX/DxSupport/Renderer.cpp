
#include "stdafx.h"
#include "Renderer.h"

#include <afxwin.h>
#include <algorithm>
#include "CommonStates.h"
#include <d3d11_1.h>
#include <DirectXColors.h>
#include "DirectXHelpers.h"
#include "DirectXPackedVector.h"
#include "DxfColorFactory.h"
#include "DxEffectColors.h"
#include "DxException.h"
#include "DxFactoryMethods.h"
#include "DxLight.h"
#include <dxgi.h>
#include <dxgi1_2.h>
#include "DxWicTextureFactory.h"
#include "Effects.h"
#include "ModelData.h"
#include "MyDxHelpers.h"
#include "PrimitiveBatch.h"
#include "RotationGroup.h"
#include "SimpleMath.h"
#include "StepTimer.h"
#include "vertexFactory.h"
#include "VertexTypes.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace DXF
{
	class RendererImp : public Renderer
	{
	private:
		// Device resources.
		HWND m_window;
		int m_outputWidth;
		int m_outputHeight;

		Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgi_factory;
		Microsoft::WRL::ComPtr< IDXGIAdapter1> m_dxgi_adapter;
		D3D_FEATURE_LEVEL m_featureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		Microsoft::WRL::ComPtr< ID3D11BlendState1> m_blendState;

		std::vector<uint32_t> m_textureColors;
		ComPtr<ID3D11Texture2D>	m_texture;
		ComPtr<ID3D11ShaderResourceView> m_textureView;

		ComPtr<ID3D11Texture2D>	m_texture2;
		ComPtr<ID3D11ShaderResourceView> m_textureView2;

		// Rendering loop timer.
		DXF::StepTimer m_timer;

		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>> m_batch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		DWORD m_nIndices;

		std::unique_ptr<DirectX::BasicEffect> m_effect2;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer2;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer2;
		DWORD m_nIndices2;
		
		DirectX::SimpleMath::Matrix m_world;
		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Matrix m_proj;

		DirectX::SimpleMath::Matrix m_world2;

		std::vector<DirectX::VertexPositionNormalTexture> m_vertices;
		std::vector<unsigned int> m_indices;

		bool m_showSecondaryModel = false;
		std::vector<DirectX::VertexPositionNormalTexture> m_vertices2;
		std::vector<unsigned int> m_indices2;
		std::wstring m_textureFile;

		RotationGroup m_rotationGroup;

		float m_nearPlaneView = 0.1f;
		float m_farPlaneView = 10.0f;

		Vertex<float> m_camera = Vertex<float>(0.f, 0.f, 0.3f);
		Vertex<float> m_target = Vertex<float>(0.0f, 0.0f, 0.0f);

		DirectX::SimpleMath::Color m_backgroundColor;

		// new colors and lights
		DxEffectColors m_effectColors;
		DxLights m_lights;

		Vertex<float> m_worldScale = Vertex<float>(0.1f, 0.1f, 0.1f);

	public:

		RendererImp() :
			m_window(nullptr),
			m_outputWidth(400),
			m_outputHeight(400),
			m_featureLevel(D3D_FEATURE_LEVEL_11_1),
			m_nIndices(0),
			m_nIndices2(0),
			m_backgroundColor(DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f))	// r,g.b,a
		{}

		void Initialize(HWND window, int width, int height) override
		{
			m_window = window;
			m_outputWidth = std::max(width, 1);
			m_outputHeight = std::max(height, 1);

			CreateDevice();

			CreateResources();

			CreateBuffers();
			CreateBuffers2();
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

			Render();
		}

		void OnWindowSizeChanged(int width, int height) override
		{
			if (!m_d3dDevice)
				return;

			m_outputWidth = std::max(width, 1);
			m_outputHeight = std::max(height, 1);

			CreateResources();
		}

		void GetDefaultSize(int& width, int& height) const override
		{
			// TODO: Change to desired default window size (note minimum size is 320x200).
			width = 800;
			height = 800;
		}

		void GetWindowSize(int& width, int& height) const override
		{
			width = m_outputWidth;
			height = m_outputHeight;
		}

		void SetModel(const DxVertexData& vertexData) override
		{
			m_vertices = vertexData.Vertices;
			m_indices = vertexData.Indices;
		}

		void SetModel2(bool show, const DxVertexData& vertexData2, const std::wstring& textureFile) override
		{
			m_showSecondaryModel = show;
			m_vertices2 = vertexData2.Vertices;
			m_indices2 = vertexData2.Indices;
			m_textureFile = textureFile;
		}

		void ResetModel() override
		{
			CreateBuffers();
		}

		void ResetModel2() override
		{
			CreateBuffers2();
			SetTexture2();
		}

		void SetRotationGroup(const RotationGroup& rg) override
		{
			m_rotationGroup = rg;
		}

		RotationGroup GetRotationGroup() override
		{
			return m_rotationGroup;
		}

		void RefreshRender(float time) override
		{
			SetWorlds(time);

			Render();
		}

		void SetTextureColors(std::vector<uint32_t> colors) override
		{
			if (!m_d3dDevice)
				return;

			if (colors.size() < 2)
				return;

			m_textureColors = colors;

			m_textureView.Reset();
			m_texture.Reset();

			DXF::ThrowIfFailed(CreateTexture2D(*m_d3dDevice.Get(), m_textureColors, m_texture,
				m_textureView), "failed to create texture");

			m_effect->SetTexture(m_textureView.Get());

			if(! CreateSecondaryWicTexture())
				SetTexture2();
		}

		void SetTexture2()
		{
			if (!m_d3dDevice)
				return;

			m_textureView2.Reset();
			m_texture2.Reset();

			if(m_showSecondaryModel)
				PrepareBackgroundTexture();
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
			if (!m_d3dDevice)
				return;

			float nearView = perspective.NearPlane;
			float farView = perspective.FarPlane;

			if (nearView > farView)
				std::swap(nearView, farView);

			m_nearPlaneView = nearView;
			m_farPlaneView = farView;

			CreateProjectionMatrix();
		}

		void SetView(const Vertex<float>& camera, const Vertex<float>& target) override
		{
			if (!m_d3dDevice)
				return;

			m_camera = camera;
			m_target = target;

			CreateViewMatrix();
		}

		void SetTarget(const Vertex<float>& target) override
		{
			if (!m_d3dDevice)
				return;

			m_target = target;

			CreateViewMatrix();
		}

		Vertex<float> GetCamera() const override
		{
			return m_camera;

		}

		Vertex<float> GetTarget() const override
		{
			return m_target;
		}

		bool IsReady() override
		{
			return (m_d3dDevice != nullptr);
		}

		bool DrawImage(CDC& dc, CSize targetSize) override
		{
			if (!m_d3dDevice)
				return nullptr;

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

			std::unique_ptr<BYTE[]> pDIBits(new BYTE[(bitmapInfo.bmiHeader.biWidth + 1) * bitmapInfo.bmiHeader.biHeight * sizeof(DWORD)]);
			VERIFY(GetDIBits(hdc3D, hbm3D, 0, bitmapInfo.bmiHeader.biHeight, pDIBits.get(), &bitmapInfo, DIB_RGB_COLORS));

			// Better quality stretching
			int oldMode = SetStretchBltMode(dc, HALFTONE);

			// This might be necessary if images don't look right
			//SetBrushOrgEx(dc, 0, 0, NULL);

			VERIFY(StretchDIBits(dc, 0, 0, targetSize.cx, targetSize.cy, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight,
				pDIBits.get(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY) != GDI_ERROR);

			pSurface1->ReleaseDC(nullptr);

			SetStretchBltMode(dc, oldMode);

			return true;
		}

		CSize GetScreenSize() const override
		{
			return CSize(m_outputWidth, m_outputHeight);
		}

		void SetBackgroundColor(const DirectX::SimpleMath::Color& bkColor) override
		{
			m_backgroundColor = bkColor;
		}

		void SetEffectColors(DxEffectColors& effectColors) override
		{
			m_effectColors = effectColors;
			SetEffectColors(*m_effect);

			if (!CreateSecondaryWicTexture())
				SetEffectColors(*m_effect2);
		}

		void SetLights(DxLights& lights) override
		{
			m_lights = lights;
			SetLighting(*m_effect);
			SetLighting(*m_effect2);
		}

		std::optional<DXF::Vertex<float>> Map2Dto3D(int x, int y) override
		{
			if (!m_d3dDevice)
				return std::nullopt;

			CD3D11_VIEWPORT viewport[1];
			UINT nViewPorts = 1;
			m_d3dContext->RSGetViewports(&nViewPorts, viewport);
			if (nViewPorts < 1)
				return std::nullopt;

			XMVECTOR pointA(XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f));
			XMVECTOR pointB(XMVectorSet(static_cast<float>(x), static_cast<float>(y), -1.0f, 0.0f));

			XMVECTOR pointATransform = XMVector3Unproject(pointA, viewport->TopLeftX, viewport->TopLeftY,
				viewport->Width, viewport->Height, viewport->MinDepth, viewport->MaxDepth,
				m_proj, m_view, m_world);

			XMVECTOR pointBTransform = XMVector3Unproject(pointB, viewport->TopLeftX, viewport->TopLeftY,
				viewport->Width, viewport->Height, viewport->MinDepth, viewport->MaxDepth,
				m_proj, m_view, m_world);

			XMVECTOR projectedVector = pointATransform - pointBTransform;

			double closestDistance = DBL_MAX;
			if (!DxHelpers::GetIntersection(m_vertices, m_indices, pointATransform, projectedVector, closestDistance))
				return std::nullopt;

			return DxHelpers::Create3DTuple(closestDistance, pointATransform, projectedVector);
		}

		void SetWorldScale(const DXF::Vertex<float>& scale) override
		{
			m_worldScale = scale;
		}

		Vertex<float> GetWorldScale() const
		{
			return m_worldScale;
		}

	protected:

		// These are the resources that depend on the device.
		void CreateDevice()
		{
			UINT creationFlags = 0;

#ifdef _DEBUG
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			static const D3D_FEATURE_LEVEL featureLevels[] =
			{
				// TODO: Modify for supported Direct3D feature levels
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
			};

			Microsoft::WRL::ComPtr<IDXGIFactory1> dxgi_factory;
			DXF::ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&dxgi_factory), "failed to create dxgi factory");

			DXF::ThrowIfFailed(dxgi_factory.As(&m_dxgi_factory), "failed to create dxgi factory1");


			// Create the DX11 API device object, and get a corresponding context.
			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> context;

			UINT i = 0;

			/* Iterate through available adapters. */
			while (m_dxgi_factory->EnumAdapters1(i++, m_dxgi_adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
			{
				HRESULT hr = D3D11CreateDevice(
					m_dxgi_adapter.Get(),                            // specify nullptr to use the default adapter
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					creationFlags,
					featureLevels,
					_countof(featureLevels),
					D3D11_SDK_VERSION,
					device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
					&m_featureLevel,                    // returns feature level of device created
					context.ReleaseAndGetAddressOf()    // returns the device immediate context
				);

				if (SUCCEEDED(hr))
					break;
			}


#ifndef NDEBUG
			ComPtr<ID3D11Debug> d3dDebug;
			if (SUCCEEDED(device.As(&d3dDebug)))
			{
				ComPtr<ID3D11InfoQueue> d3dInfoQueue;
				if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
				{
#ifdef _DEBUG
					d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
					d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
					D3D11_MESSAGE_ID hide[] =
					{
						D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
						// TODO: Add more message IDs here as needed.
					};
					D3D11_INFO_QUEUE_FILTER filter = {};
					filter.DenyList.NumIDs = 1;					// # of items in hide
					filter.DenyList.pIDList = hide;
					d3dInfoQueue->AddStorageFilterEntries(&filter);
				}
			}
#endif
			DXF::ThrowIfFailed(device.As(&m_d3dDevice), "device 1 not supported");
			DXF::ThrowIfFailed(context.As(&m_d3dContext), "context 1 not supported");

			CreateBlendState();

			// TODO: Initialize device dependent objects here (independent of window size).
			m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

			m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());

			m_effect->SetTextureEnabled(true);

			SetEffectColors(*m_effect);
			SetLighting(*m_effect);

			void const* shaderByteCode;
			size_t byteCodeLength;

			m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

			DXF::ThrowIfFailed(
				m_d3dDevice->CreateInputLayout(VertexPositionNormalTexture::InputElements,
					VertexPositionNormalTexture::InputElementCount,
					shaderByteCode, byteCodeLength,
					m_inputLayout.ReleaseAndGetAddressOf()), "failed to create input layout");

			if (m_textureColors.empty())
				m_textureColors = CreateRainbow();

			DXF::ThrowIfFailed(CreateTexture2D(*m_d3dDevice.Get(), m_textureColors, m_texture,
				m_textureView), "failed to create texture");

			m_effect->SetTexture(m_textureView.Get());

			CreateBackgroundTexture();		

			m_world = Matrix::Identity;
			m_world2 = Matrix::Identity;
		}

		bool CreateSecondaryWicTexture()
		{
			return ! m_textureFile.empty();
		}

		void CreateBackgroundTexture()
		{
			m_effect2 = std::make_unique<BasicEffect>(m_d3dDevice.Get());

			m_effect2->SetTextureEnabled(true);

			SetEffectColors(*m_effect2);
			SetLighting(*m_effect2);

			void const* shaderByteCode;
			size_t byteCodeLength;

			m_effect2->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

			if(m_showSecondaryModel)
				PrepareBackgroundTexture();
		}

		void PrepareBackgroundTexture()
		{
			if (m_textureFile.empty())
			{
				if (m_textureColors.empty())
					return;

				DXF::ThrowIfFailed(CreateTexture2D(*m_d3dDevice.Get(), m_textureColors, m_texture2,
						m_textureView2), "failed to create texture2");
			}
			else
				DXF::ThrowIfFailed(WicTextureFactory::CreateWicTexture2D(*m_d3dDevice.Get(), *m_d3dContext.Get(),
					m_textureFile, m_texture2, m_textureView2), "failed to create wic texture2");

			m_effect2->SetTexture(m_textureView2.Get());
		}

		void CreateProjectionMatrix()
		{
			m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
				static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight), m_nearPlaneView, m_farPlaneView);
		}

		void CreateViewMatrix()
		{
			m_view = Matrix::CreateLookAt(
				Vector3(m_camera.X, m_camera.Y, m_camera.Z),
				Vector3::Zero,
				Vector3::UnitY);
		}

		// Allocate all memory resources that change on a window SizeChanged event.
		void CreateResources()
		{
			// Clear the previous window size specific context.
			ID3D11RenderTargetView* nullViews[] = { nullptr };
			m_d3dContext->OMSetRenderTargets(0, nullViews, nullptr);
			m_renderTargetView.Reset();
			m_depthStencilView.Reset();
			m_d3dContext->Flush();

			UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
			UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
			DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
			DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
			UINT backBufferCount = 2;

			// If the swap chain already exists, resize it, otherwise create one.
			if (m_swapChain)
			{
				HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, swapChainFlags);

				if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				{
					// If the device was removed for any reason, a new device and swap chain will need to be created.
					OnDeviceLost();

					// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
					// and correctly set up the new device.
					return;
				}
				else
				{
					DXF::ThrowIfFailed(hr, "failed to resize swap chain buffer");
				}
			}
			else
			{
				// First, retrieve the underlying DXGI Device from the D3D Device.
				ComPtr<IDXGIDevice1> dxgiDevice;
				DXF::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice), "device 1 not supported");

				// Identify the physical adapter (GPU or card) this device is running on.
				ComPtr<IDXGIAdapter> dxgiAdapter;
				DXF::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()), "could not get adapter");

				// And obtain the factory object that created it.
				ComPtr<IDXGIFactory2> dxgiFactory;
				DXF::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())), "could not get factory 2 from adapter");

				// Create a descriptor for the swap chain.
				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = backBufferWidth;
				swapChainDesc.Height = backBufferHeight;
				swapChainDesc.Format = backBufferFormat;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = backBufferCount;
				swapChainDesc.Flags = swapChainFlags;

				DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
				fsSwapChainDesc.Windowed = TRUE;

				// Create a SwapChain from a Win32 window.
				DXF::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
					m_d3dDevice.Get(),
					m_window,
					&swapChainDesc,
					&fsSwapChainDesc,
					nullptr,
					m_swapChain.ReleaseAndGetAddressOf()
				), "failed to create swap chain");

				// This template does not support exclusive full screen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
				DXF::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER), "failed to associate window");
			}

			// Obtain the backbuffer for this window which will be the final 3D render target.
			ComPtr<ID3D11Texture2D> backBuffer;
			DXF::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())), "failed to get back buffer");

			// Create a view interface on the render target to use on bind.
			DXF::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr,
				m_renderTargetView.ReleaseAndGetAddressOf()), "failed to create render target view");

			// Allocate a 2-D surface as the depth/stencil buffer and
			// create a DepthStencil view on this surface to use on bind.
			CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
			//			depthStencilDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

			ComPtr<ID3D11Texture2D> depthStencil;
			DXF::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()),
				"failed to create texture");

			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			DXF::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc,
				m_depthStencilView.ReleaseAndGetAddressOf()), "failed to create stencil view");

			CreateViewMatrix();

			CreateProjectionMatrix();
		}

		void CreateBlendState()
		{
			D3D11_BLEND_DESC1 blendStateDesc;
			blendStateDesc.AlphaToCoverageEnable = FALSE;
			blendStateDesc.IndependentBlendEnable = FALSE;
			blendStateDesc.RenderTarget[0].LogicOpEnable = FALSE;			// not sure what this does
			blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			blendStateDesc.RenderTarget[0].LogicOp = D3D11_LOGIC_OP_NOOP;

			ComPtr<ID3D11BlendState1> blendState;
			DXF::ThrowIfFailed(m_d3dDevice->CreateBlendState1(&blendStateDesc, m_blendState.ReleaseAndGetAddressOf()),
				"Could not create blend state");
		}

		HRESULT CreateBuffers()
		{
			if (!m_d3dDevice)
				return E_UNEXPECTED;

			if (m_vertices.empty())
				return S_OK;

			HRESULT hr = CreateImmutableTextureVertexBuffer(*m_d3dDevice.Get(), m_vertices,
				m_vertexBuffer);

			if (FAILED(hr))
				return hr;

			return CreateImmutableIndexBuffer(*m_d3dDevice.Get(), m_indices,
				m_indexBuffer, m_nIndices);
		}

		HRESULT CreateBuffers2()
		{
			if (!m_showSecondaryModel)
				return S_OK;

			if (!m_d3dDevice)
				return E_UNEXPECTED;

			if (m_vertices2.empty())
				return S_OK;

			HRESULT hr = CreateImmutableTextureVertexBuffer(*m_d3dDevice.Get(), m_vertices2,
				m_vertexBuffer2);

			if (FAILED(hr))
				return hr;

			return CreateImmutableIndexBuffer(*m_d3dDevice.Get(), m_indices2,
				m_indexBuffer2, m_nIndices2);
		}

		void OnDeviceLost()
		{
			m_indexBuffer.Reset();
			m_indexBuffer2.Reset();
			m_vertexBuffer.Reset();
			m_vertexBuffer2.Reset();
			m_states.reset();
			m_effect.reset();

			m_textureView.Reset();
			m_texture.Reset();
			m_effect2.reset();

			m_textureView2.Reset();
			m_texture2.Reset();

			m_inputLayout.Reset();

			m_depthStencilView.Reset();
			m_renderTargetView.Reset();
			m_swapChain.Reset();
			m_blendState.Reset();
			m_d3dContext.Reset();
			m_d3dDevice.Reset();
			m_nIndices = 0;
			m_nIndices2 = 0;

			CreateDevice();

			CreateResources();
		}

		void SetEffectColors(BasicEffect& effect)
		{
			effect.SetAmbientLightColor(m_effectColors.AmbientColor);
			effect.SetDiffuseColor(m_effectColors.DiffuseColor);
			effect.SetSpecularColor(m_effectColors.SpecularColor);
			effect.SetEmissiveColor(m_effectColors.EmissiveColor);
			effect.SetSpecularPower(m_effectColors.SpecularPower);
		}

		void SetLight(BasicEffect& effect, int lightIndex, const DxLight& light)
		{
			assert(lightIndex >= 0);
			assert(lightIndex < 3);
			if (lightIndex < 0 || lightIndex > 2)
				return;

			effect.SetLightEnabled(lightIndex, light.Enable);
			if (light.Enable)
			{
				effect.SetLightDiffuseColor(lightIndex, light.DiffuseColor);
				effect.SetLightSpecularColor(lightIndex, light.SpectacularColor);
				effect.SetLightDirection(lightIndex, light.Direction);
			}
		}

		void SetLighting(BasicEffect& effect)
		{
			effect.SetLightingEnabled(true);
			effect.SetVertexColorEnabled(false);	// this has to be false
			effect.SetPerPixelLighting(m_lights.PerPixelLighting);

			if (m_lights.DefaultLights)
			{
				effect.EnableDefaultLighting();
			}
			else
			{
				effect.SetAmbientLightColor(m_effectColors.AmbientColor);
				for (int i = 0; i < 3; ++i)
					SetLight(effect, i, m_lights.Lights.at(i));
			}
		}

		// Updates the world.
		void Update(DXF::StepTimer const& timer)
		{
			float time = float(timer.GetTotalSeconds());

			SetWorlds(time);
		}

		static float RoundDegrees(float deg)
		{
			return fmod(deg, 360.0f);
		}

		void SetWorlds(float time)
		{
			SetWorld1(time);
			SetWorld2();
		}

		void SetWorld1(float time)
		{
			float radians = time / 2;
			float degrees = XMConvertToDegrees(radians);
			auto transMatrix = Matrix::CreateTranslation(m_target.X, m_target.Y, m_target.Z);

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

			m_world =
				Matrix::CreateScale(m_worldScale.X, m_worldScale.Y, m_worldScale.Z) *
				Matrix::CreateRotationY(XMConvertToRadians(m_rotationGroup.RotationParamsMain.AngleYDegrees)) *
				Matrix::CreateRotationX(XMConvertToRadians(m_rotationGroup.RotationParamsMain.AngleXDegrees)) *
				Matrix::CreateRotationZ(XMConvertToRadians(m_rotationGroup.RotationParamsMain.AngleZDegrees)) *
				transMatrix;
		}

		void SetWorld2()
		{
			if (m_rotationGroup.RotationType == RotationSelectionType::LockBackgroundOnModel)
			{
				m_world2 = m_world;
				return;
			}
				
			auto transMatrix = Matrix::CreateTranslation(m_target.X, m_target.Y, m_target.Z);

			m_world2 =
				Matrix::CreateScale(m_worldScale.X, m_worldScale.Y, m_worldScale.Z) *
				Matrix::CreateRotationY(XMConvertToRadians(m_rotationGroup.RotationParamsBackground.AngleYDegrees)) *
				Matrix::CreateRotationX(XMConvertToRadians(m_rotationGroup.RotationParamsBackground.AngleXDegrees)) *
				Matrix::CreateRotationZ(XMConvertToRadians(m_rotationGroup.RotationParamsBackground.AngleZDegrees)) *
				transMatrix;
		}

		void Render()
		{
			if (!m_d3dContext)
				return;

			if (!m_textureView)
				return;

			if (!m_vertexBuffer)
				return;

			Clear();

			m_effect->SetView(m_view);
			m_effect->SetProjection(m_proj);
			m_effect->SetWorld(m_world);

			float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_d3dContext->OMSetBlendState(m_blendState.Get(), blendFactors, 0xFFFFFFFF);

			m_d3dContext->RSSetState(m_states->CullNone());

			m_effect->Apply(m_d3dContext.Get());
			
			ID3D11SamplerState* samplers[] = { m_states->AnisotropicClamp() };
			m_d3dContext->PSSetSamplers(0, 1, samplers);

			RenderPrimaryModel();

			if (m_showSecondaryModel)
			{
				m_effect2->SetView(m_view);
				m_effect2->SetProjection(m_proj);
				m_effect2->SetWorld(m_world2);

				m_effect2->Apply(m_d3dContext.Get());

				RenderSecondaryModel();
			}

			Present();
		}

		void RenderPrimaryModel()
		{
			UINT stride = sizeof(VertexPositionNormalTexture);
			UINT offset = 0;
			m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

			m_d3dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_d3dContext->IASetInputLayout(m_inputLayout.Get());

			m_d3dContext->DrawIndexed(m_nIndices, 0, 0);
		}

		void RenderSecondaryModel()
		{
			if (!m_vertexBuffer2)
				return;

			UINT stride = sizeof(VertexPositionNormalTexture);
			UINT offset = 0;

			m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer2.GetAddressOf(), &stride, &offset);

			m_d3dContext->IASetIndexBuffer(m_indexBuffer2.Get(), DXGI_FORMAT_R32_UINT, 0);

			m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_d3dContext->IASetInputLayout(m_inputLayout.Get());

			m_d3dContext->DrawIndexed(m_nIndices2, 0, 0);
		}

		void Clear()
		{
			// Clear the views.
			m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), m_backgroundColor); // Colors::CornflowerBlue);
			m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

			// Set the viewport.
			CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
			m_d3dContext->RSSetViewports(1, &viewport);
		}

		// Presents the back buffer contents to the screen.
		void Present()
		{
			// The first argument instructs DXGI to block until VSync, putting the application
			// to sleep until the next VSync. This ensures we don't waste any cycles rendering
			// frames that will never be displayed to the screen.
			HRESULT hr = m_swapChain->Present(1, 0);

			// If the device was reset we must completely reinitialize the renderer.
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				OnDeviceLost();
			}
			else
			{
				DXF::ThrowIfFailed(hr, "device lost during present");
			}
		}

	};

	std::shared_ptr<Renderer> Renderer::CreateRenderer()
	{
		return std::make_shared<RendererImp>();
	}


}












