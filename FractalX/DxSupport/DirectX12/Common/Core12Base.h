#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_4.h>

// Adapted from Luna Dx12 Box6 project d3dApp.h without window instance 

class Core12Base
{
protected:

	Core12Base();
	Core12Base(const Core12Base& rhs) = delete;
	Core12Base& operator=(const Core12Base& rhs) = delete;
	virtual ~Core12Base();

public:
	HWND MainWnd()const;
	float AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	virtual bool Initialize(HWND mainWnd, int width, int height);
	virtual void Resize(int width, int height);
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM /*btnState*/, int /*x*/, int /*y*/) { }
	virtual void OnMouseUp(WPARAM /*btnState*/, int /*x*/, int /*y*/) { }
	virtual void OnMouseMove(WPARAM /*btnState*/, int /*x*/, int /*y*/) { }

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();

protected:

	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();

	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

	HWND m_hWnd = nullptr;			// main window handle
	bool m_Resizing = false;

	// Set true to use 4X MSAA (§4.1.8).  The default is false.
	bool m_4xMsaaState = false;		// 4X MSAA enabled
	UINT m_4xMsaaQuality = 0;		// quality level of 4X MSAA


	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_currentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_directCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

	static const int SwapChainBufferCount = 2;
	int m_currBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

	D3D12_VIEWPORT m_screenViewport;
	D3D12_RECT m_scissorRect;

	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvSrvUavDescriptorSize = 0;

	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int m_clientWidth = 800;
	int m_clientHeight = 600;
};
