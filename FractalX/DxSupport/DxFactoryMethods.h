#pragma once

#include <basetyps.h>
#include <wrl\client.h>

interface ID3D11Device1;
interface ID3D11Texture2D;
interface ID3D11ShaderResourceView;
struct ID3D11Buffer;

namespace DirectX
{
	struct VertexPositionNormalTexture;
}

namespace DXF
{
	HRESULT CreateTexture2D(ID3D11Device1& device, std::vector<uint32_t>& colors, 
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView);

	HRESULT CreateImmutableTextureVertexBuffer(ID3D11Device1& device, const std::vector<DirectX::VertexPositionNormalTexture>& vertices,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer);

	HRESULT CreateImmutableIndexBuffer(ID3D11Device1& device, const std::vector<unsigned int>& indices,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer, DWORD& nIndices);
}