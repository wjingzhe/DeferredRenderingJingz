#pragma once

#include <d3d11.h>

class CombineFinalRenderer
{
public:
	CombineFinalRenderer();
	~CombineFinalRenderer();

	void Render(ID3D11RenderTargetView* pFinalRT, ID3D11ShaderResourceView* pAlbedo, ID3D11ShaderResourceView* pLight);

private:

	ID3D11PixelShader* m_pCombinePixelShader;

};
