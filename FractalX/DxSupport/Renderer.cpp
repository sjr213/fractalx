
#include "stdafx.h"
#include "Renderer.h"

#include <afxwin.h>
#include <algorithm>
#include "CommonStates.h"
#include <d3d11_1.h>
#include <DirectXColors.h>
#include "DirectXHelpers.h"
#include "DxfColorFactory.h"
#include "DxEffectColors.h"
#include "DxException.h"
#include "DxFactoryMethods.h"
#include <dxgi.h>
#include <dxgi1_2.h>
#include "Effects.h"
#include "ModelData.h"
#include "PrimitiveBatch.h"
#include "RotationParams.h"
#include "SimpleMath.h"
#include "StepTimer.h"
#include "vertexFactory.h"
#include "VertexTypes.h"
#include <DirectXPackedVector.h>

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

		// Rendering loop timer.
		DXF::StepTimer m_timer;

		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>> m_batch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		
		DWORD m_nIndices;

		DirectX::SimpleMath::Matrix m_world;
		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Matrix m_proj;

		std::vector<DirectX::VertexPositionNormalTexture> m_vertices;
		std::vector<unsigned int> m_indices;

		RotationParams m_rotationParams;

		float m_nearPlaneView = 0.1f;
		float m_farPlaneView = 10.0f;

		std::tuple<float, float, float> m_camera = std::make_tuple(0.f, 0.f, 0.3f);
		std::tuple<float, float, float> m_target = std::make_tuple(0.0f, 0.0f, 0.0f);

		DirectX::SimpleMath::Color m_backgroundColor;

		// new colors and lights
		DXF::DxEffectColors m_effectColors;

	public:

		RendererImp() :
			m_window(nullptr),
			m_outputWidth(400),
			m_outputHeight(400),
			m_featureLevel(D3D_FEATURE_LEVEL_11_1),
			m_nIndices(0),
			m_rotationParams(RotationAction::RotateY, 0.0, 0.0, 0.0),
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
			height = 600;
		}

		void SetModel(const DxVertexData& vertexData) override
		{
			m_vertices = vertexData.Vertices;
			m_indices = vertexData.Indices;
		}

		void ResetModel() override
		{
			CreateBuffers();
		}

		void SetRotationParams(const RotationParams& rp) override
		{
			m_rotationParams = rp;
		}

		void RefreshRender(float time) override
		{
			SetWorld(time);

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

		void SetView(const std::tuple<float, float, float>& camera, const std::tuple<float, float, float>& target) override
		{
			if (!m_d3dDevice)
				return;

			m_camera = camera;
			m_target = target;

			CreateViewMatrix();
		}

		void SetTarget(const std::tuple<float, float, float>& target) override
		{
			if (!m_d3dDevice)
				return;

			m_target = target;

			CreateViewMatrix();
		}

		std::tuple<float, float, float> GetCamera() const override
		{
			return m_camera;

		}

		std::tuple<float, float, float> GetTarget() const override
		{
			return m_target;
		}

		bool IsReady() override
		{
			return (m_d3dDevice != nullptr);
		}

		bool DrawImage(CDC& dc, CSize imageSize) override
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

			VERIFY(GetDIBits(hdc3D, hbm3D, 0, imageSize.cy, nullptr, &bitmapInfo, DIB_RGB_COLORS));

			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			bitmapInfo.bmiHeader.biWidth = imageSize.cx;
			bitmapInfo.bmiHeader.biHeight = imageSize.cy;

			std::unique_ptr<BYTE[]> pDIBits(new BYTE[(bitmapInfo.bmiHeader.biWidth + 1) * bitmapInfo.bmiHeader.biHeight * sizeof(DWORD)]);
			VERIFY(GetDIBits(hdc3D, hbm3D, 0, imageSize.cy, pDIBits.get(), &bitmapInfo, DIB_RGB_COLORS));

			VERIFY(StretchDIBits(dc, 0, 0, imageSize.cx, imageSize.cy, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight,
				pDIBits.get(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY) != GDI_ERROR);

			pSurface1->ReleaseDC(nullptr);

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

		void SetEffectColors(DXF::DxEffectColors& effectColors) override
		{
			m_effectColors = effectColors;
			SetEffectColors(*m_effect);
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
			DXF::ThrowIfFailed( CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&dxgi_factory), "failed to create dxgi factory");

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
			SetLights(*m_effect);

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

			m_world = Matrix::Identity;
		}

		void CreateProjectionMatrix()
		{
			m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
				static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight), m_nearPlaneView, m_farPlaneView);
		}

		void CreateViewMatrix()
		{
			m_view = Matrix::CreateLookAt(
				Vector3(std::get<0>(m_camera), std::get<1>(m_camera), std::get<2>(m_camera)),
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

		void OnDeviceLost()
		{
			m_indexBuffer.Reset();
			m_vertexBuffer.Reset();
			m_states.reset();
			m_effect.reset();

			m_textureView.Reset();
			m_texture.Reset();

			m_inputLayout.Reset();

			m_depthStencilView.Reset();
			m_renderTargetView.Reset();
			m_swapChain.Reset();
			m_blendState.Reset();
			m_d3dContext.Reset();
			m_d3dDevice.Reset();
			m_nIndices = 0;

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

		void SetLights(BasicEffect& effect)
		{
			effect.SetLightingEnabled(true);
			effect.EnableDefaultLighting();
			effect.SetVertexColorEnabled(false);	// this has to be false
			effect.SetPerPixelLighting(false);
		}

		// Updates the world.
		void Update(DXF::StepTimer const& timer)
		{
			float time = float(timer.GetTotalSeconds());

			SetWorld(time);
		}

		void SetWorld(float time)
		{
			auto transMatrix = Matrix::CreateTranslation(std::get<0>(m_target), std::get<1>(m_target), std::get<2>(m_target));
			if (m_rotationParams.Action == RotationAction::Fixed)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees)) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees)) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees)) *
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateX)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees)) *
					Matrix::CreateRotationX(time) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees)) *
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateY)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(time) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees)) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees)) *
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateZ)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees)) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees)) *
					Matrix::CreateRotationZ(time) *
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateXY)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees) + time) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees) + time) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees)) *
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateYZ)
			{
				m_world = transMatrix * 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees) + time) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees)) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees) + time)*
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateXZ)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees)) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees) + time) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees) + time) *
					transMatrix;
			}
			else if (m_rotationParams.Action == RotationAction::RotateAll)
			{
				m_world = 
					Matrix::CreateScale(0.1f, 0.1f, 0.1f) *
					Matrix::CreateRotationY(XMConvertToRadians(m_rotationParams.AngleYDegrees) + time) *
					Matrix::CreateRotationX(XMConvertToRadians(m_rotationParams.AngleXDegrees) + time) *
					Matrix::CreateRotationZ(XMConvertToRadians(m_rotationParams.AngleZDegrees) + time) *
					transMatrix;
			}
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

			UINT stride = sizeof(VertexPositionNormalTexture);
			UINT offset = 0;
			m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

			m_d3dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_d3dContext->IASetInputLayout(m_inputLayout.Get());

			m_d3dContext->DrawIndexed(m_nIndices, 0, 0);

			Present();
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












