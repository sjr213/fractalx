
#include "stdafx.h"
#include "DxFactoryMethods.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include "VertexTypes.h"
#include <winerror.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

namespace DXF
{
	HRESULT CreateTexture2D(ID3D11Device1& device, std::vector<uint32_t>& colors, ComPtr<ID3D11Texture2D>& texture,
		ComPtr<ID3D11ShaderResourceView>& textureView)
	{
		textureView.Reset();
		texture.Reset();

		if (colors.empty())
			return E_FAIL;

		UINT width = static_cast<UINT>(colors.size());
		UINT height = 1;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = colors.data();
		initData.SysMemPitch = width * sizeof(uint32_t);
		initData.SysMemSlicePitch = 0;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;

		desc.Usage = D3D11_USAGE_DEFAULT;				//  D3D11_USAGE_IMMUTABLE
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;						// we won't change in the CPU, just recreate if needed

		HRESULT hr = device.CreateTexture2D(&desc, &initData, texture.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;

			hr = device.CreateShaderResourceView(texture.Get(),
				&SRVDesc, textureView.GetAddressOf());
		}

		return hr;
	}

	HRESULT CreateImmutableTextureVertexBuffer(ID3D11Device1& device, const vector<VertexPositionNormalTexture>& vertices,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer)
	{
		auto vBufLen = static_cast<UINT>(vertices.size() * sizeof(VertexPositionNormalTexture));

		D3D11_BUFFER_DESC vertexDescBuffer;
		vertexDescBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDescBuffer.ByteWidth = vBufLen;
		vertexDescBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDescBuffer.CPUAccessFlags = 0;
		vertexDescBuffer.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vertices.data();

		return device.CreateBuffer(&vertexDescBuffer, &vertexData, &vertexBuffer);
	}

	HRESULT CreateImmutableIndexBuffer(ID3D11Device1& device, const vector<unsigned int>& indices,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer, DWORD& nIndices)
	{
		nIndices = static_cast<DWORD>(indices.size());
		auto iBufLen = static_cast<UINT>(sizeof(unsigned int) * nIndices);

		D3D11_BUFFER_DESC indexDescBuffer;
		indexDescBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		indexDescBuffer.ByteWidth = iBufLen;
		indexDescBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDescBuffer.CPUAccessFlags = 0;
		indexDescBuffer.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices.data();

		return device.CreateBuffer(&indexDescBuffer, &indexData, &indexBuffer);
	}
}