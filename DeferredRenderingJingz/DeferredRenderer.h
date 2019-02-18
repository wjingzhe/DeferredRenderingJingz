#pragma once
#include <d3d11.h>
#include "GBufferRenderer.h"
#include "LightningRenderer.h"
#include "CombineFinalRenderer.h"

class Camera;
class Scene;

class DeferredRenderer
{
public:
	DeferredRenderer(void);
	~DeferredRenderer(void);

	void Update();
	void Render();



protected:
private:

	GBufferRenderer * m_pGBufferRenderer;
	LightingRenderer * m_pDirectionalLightingRenderer;
	CombineFinalRenderer* m_pCombineFinalRenderer;

	// Albedo
	ID3D11Texture2D* m_pAlbedoTexture;
	ID3D11RenderTargetView* m_pAlbedoRTV;
	ID3D11ShaderResourceView* m_pAlbedoSRV;

	//Normal
	ID3D11Texture2D* m_pNormalTexture;
	ID3D11RenderTargetView* m_pNormalRTV;
	ID3D11ShaderResourceView* m_pNormalSRV;

	//Depth
	ID3D11Texture2D* m_pDepthTexture;
	ID3D11RenderTargetView* m_pDepthRTV;
	ID3D11ShaderResourceView* m_pDepthSRV;

	// LightMap
	ID3D11Texture2D* m_pLightTexture;
	ID3D11RenderTargetView* m_pLightRTV;
	ID3D11ShaderResourceView* m_pLightSRV;

	// Final Render Target
	ID3D11Texture2D* m_pFinalTexture;
	ID3D11RenderTargetView* m_pFinalRTV;
	ID3D11ShaderResourceView* m_pFinalSRV;

	//
	ID3D11PixelShader* m_pDrawTexturePixelShader_UnUsed;
	ID3D11SamplerState* m_pLinearSampler;

	Camera* m_pCamera;
	Scene* m_pScene;

};