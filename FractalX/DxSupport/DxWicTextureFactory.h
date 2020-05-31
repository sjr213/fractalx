#pragma once

#include <basetyps.h>
#include <wrl/client.h>

interface ID3D11Device1;
interface ID3D11DeviceContext1;
interface ID3D11Texture2D;
interface ID3D11ShaderResourceView;

namespace DXF::WicTextureFactory
{
	HRESULT CreateWicTexture2D(ID3D11Device1& device, ID3D11DeviceContext1& context, std::wstring& filename, 
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView);
}