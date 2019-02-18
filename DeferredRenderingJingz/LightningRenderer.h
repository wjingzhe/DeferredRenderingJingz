#pragma once

#include <d3d11.h>
#include "ConstantBufferStructs.h"
#include "Camera.h"

class LightingRenderer
{
public:
	LightingRenderer();
	~LightingRenderer();

	void Render(ID3D11RenderTargetView* pLightMapRT,
		ID3D11ShaderResourceView* pAlbedo,
		ID3D11ShaderResourceView* pNormal,
		ID3D11ShaderResourceView* pDepth,
		Camera* pCamera);

private:
	ID3D11PixelShader* m_pLightingPixelShader;

	LightingConstantBuffer m_LightConstantBufferStruct;
	ID3D11Buffer* m_pConstantBuffer;
};
