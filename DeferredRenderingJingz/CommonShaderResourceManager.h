#pragma once

#include <d3d11.h>
#include "ScreenSolution.h"

class CommonShaderResourceManager
{
public:

	static CommonShaderResourceManager* GetInstance();
	ID3D11Device* GetDevice();

	ScreenSolution* GetScreenSolution(void)
	{
		return &m_ScreenSolution;
	}

	ID3D11DeviceContext* GetImmediateContext();
	ID3D11RenderTargetView* GetBackBufferRenderTarget();
	ID3D11DepthStencilView* GetDepthStencil();
	ID3D11SamplerState* GetSamplerLinear();
	ID3D11SamplerState* GetSamplerPoint();

	//Tools.
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	ID3D11PixelShader* CreatePixelShader(WCHAR* pFileName, LPCSTR pEntryPoint, ID3DBlob* pPixelShaderBlobOut);
	ID3D11PixelShader* CreatePixelShader(WCHAR* pFileName, LPCSTR pEntryPoint);
	ID3D11VertexShader* CreateVertexShader(WCHAR* pFileName, LPCSTR pEntryPoint, ID3DBlob** ppVertexShaderBlobOut);

	void ClearDepthStencilView();
	void Present();
	void SetOpaque();

private:
	CommonShaderResourceManager();
	~CommonShaderResourceManager();

	static CommonShaderResourceManager* s_pInstance;

	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;
	D3D_DRIVER_TYPE m_DriveType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	ID3D11RenderTargetView* m_pBackBufferRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilTexture;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11RasterizerState* m_pRasterizerState;
	ID3D11SamplerState* m_pSamplerLinear;
	ID3D11SamplerState* m_pSamplerPoint;

	ID3D11BlendState* m_pOpaqueBlendState;
	ID3D11BlendState* m_pAlphaBlendState;

	ScreenSolution m_ScreenSolution;
};
