#pragma once

#include <d3d11.h>
#include "ConstantBufferStructs.h"
#include "Camera.h"
#include "Scene.h"

class GBufferRenderer
{
public:
	GBufferRenderer(void);
	~GBufferRenderer(void);

	void Render(ID3D11RenderTargetView* pAlbedo, ID3D11RenderTargetView* pNormal, ID3D11RenderTargetView* pDepth, Camera* pCamera, Scene* pScene);

protected:
	void ClearRenderTargetViews(ID3D11RenderTargetView * pAlbedo, ID3D11RenderTargetView * pNormal, ID3D11RenderTargetView * pDepth);

protected:

	ID3D11VertexShader* m_pRenderGBufferVertexShader;
	ID3D11PixelShader* m_pRenderGBufferPixelShader;

	ID3D11Buffer* m_pConstantBuffer;

	ID3D11InputLayout* m_pVertexInputLayout;

	ID3D11Buffer* m_pInstancedVertexBuffer;

private:
};