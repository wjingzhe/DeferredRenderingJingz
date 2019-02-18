#include "DeferredRenderer.h"
#include "CommonShaderResourceManager.h"
#include "QuadRenderer.h"
#include "Camera.h"
#include "Scene.h"
#include "InputManager.h"

using namespace DirectX;

DeferredRenderer::DeferredRenderer(void)
{
	HRESULT hr = S_OK;

	UINT width = CommonShaderResourceManager::GetInstance()->GetScreenSolution()->GetScreenWidth();
	UINT height = CommonShaderResourceManager::GetInstance()->GetScreenSolution()->GetScreenHeight();

	auto pDevice = CommonShaderResourceManager::GetInstance()->GetDevice();

	//Create the albedo texture
	D3D11_TEXTURE2D_DESC albedoTexDesc;
	ZeroMemory(&albedoTexDesc, sizeof(albedoTexDesc));
	albedoTexDesc.Width = width;
	albedoTexDesc.Height = height;
	albedoTexDesc.MipLevels = 1;
	albedoTexDesc.ArraySize = 1;
	albedoTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	albedoTexDesc.SampleDesc.Count = 1;
	albedoTexDesc.SampleDesc.Quality = 0;
	albedoTexDesc.Usage = D3D11_USAGE_DEFAULT;
	albedoTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	albedoTexDesc.CPUAccessFlags = 0;
	albedoTexDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&albedoTexDesc, NULL, &m_pAlbedoTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating albedo texture.", L"Error", MB_OK);
	}
	m_pAlbedoTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("AlbedoTexture"), "AlbedoTexture");


	// Create the albedo render target.
	D3D11_RENDER_TARGET_VIEW_DESC albedoRTVDesc;
	ZeroMemory(&albedoRTVDesc, sizeof(albedoRTVDesc));
	albedoRTVDesc.Format = albedoTexDesc.Format;
	albedoRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	albedoRTVDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(m_pAlbedoTexture, &albedoRTVDesc, &m_pAlbedoRTV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating albedo RT.", L"Error", MB_OK);
	}
	m_pAlbedoRTV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("AlbedoRT"), "AlbedoRT");

	// Create the albedo shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC albedoSRVDesc;
	albedoSRVDesc.Format = albedoTexDesc.Format;
	albedoSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	albedoSRVDesc.Texture2D.MostDetailedMip = 0;
	albedoSRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pAlbedoTexture, &albedoSRVDesc, &m_pAlbedoSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating albedo SRV.", L"Error", MB_OK);
	}
	m_pAlbedoSRV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("AlbedoSRV"), "AlbedoSRV");


	//Normal

	// Create the normal texture.
	D3D11_TEXTURE2D_DESC normalTextureDesc;
	ZeroMemory(&normalTextureDesc, sizeof(normalTextureDesc));
	normalTextureDesc.Width = width;
	normalTextureDesc.Height = height;
	normalTextureDesc.MipLevels = 1;
	normalTextureDesc.ArraySize = 1;
	normalTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	normalTextureDesc.SampleDesc.Count = 1;
	normalTextureDesc.SampleDesc.Quality = 0;
	normalTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	normalTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	normalTextureDesc.CPUAccessFlags = 0;
	normalTextureDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&normalTextureDesc, NULL, &m_pNormalTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating normal texture.", L"Error", MB_OK);
	}
	m_pNormalTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("NormalTexture"), "NormalTexture");

	D3D11_RENDER_TARGET_VIEW_DESC normalRTVDesc;
	ZeroMemory(&normalRTVDesc, sizeof(normalRTVDesc));
	normalRTVDesc.Format = normalTextureDesc.Format;
	normalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	normalRTVDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(m_pNormalTexture, &normalRTVDesc, &m_pNormalRTV);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating normal RT.", L"Error", MB_OK);
	}
	m_pNormalRTV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("NormalRT"), "NormalRT");

	// Create the Normal shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC normalSRVDesc;
	normalSRVDesc.Format = normalTextureDesc.Format;
	normalSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	normalSRVDesc.Texture2D.MostDetailedMip = 0;
	normalSRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pNormalTexture, &normalSRVDesc, &m_pNormalSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating normal SRV.", L"Error", MB_OK);
	}
	m_pNormalSRV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("NormalSRV"), "NormalSRV");

	//Depth

	//Create the depth texture.
	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = width;
	depthTextureDesc.Height = height;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&depthTextureDesc, NULL, &m_pDepthTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating depth texture.", L"Error", MB_OK);
	}
	m_pDepthTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("DepthTexture"), "DepthTexture");

	//Create the depth RT
	D3D11_RENDER_TARGET_VIEW_DESC DepthRTVDesc;
	ZeroMemory(&DepthRTVDesc, sizeof(DepthRTVDesc));
	DepthRTVDesc.Format = depthTextureDesc.Format;
	DepthRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DepthRTVDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(m_pDepthTexture, &DepthRTVDesc, &m_pDepthRTV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating depth RT.", L"Error", MB_OK);
	}
	m_pDepthRTV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("DepthRT"), "DepthRT");

	// Create the depth shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc;
	depthSRVDesc.Format = depthTextureDesc.Format;
	depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSRVDesc.Texture2D.MostDetailedMip = 0;
	depthSRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pDepthTexture, &depthSRVDesc, &m_pDepthSRV);
	m_pDepthSRV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("DepthSRV"), "DepthSRV");


	// LightMap.
	// Create the light texture
	D3D11_TEXTURE2D_DESC LightTextureDesc;
	ZeroMemory(&LightTextureDesc, sizeof(LightTextureDesc));
	LightTextureDesc.Width = width;
	LightTextureDesc.Height = height;
	LightTextureDesc.MipLevels = 1;
	LightTextureDesc.ArraySize = 1;
	LightTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	LightTextureDesc.SampleDesc.Count = 1;
	LightTextureDesc.SampleDesc.Quality = 0;
	LightTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	LightTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	LightTextureDesc.CPUAccessFlags = 0;
	LightTextureDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&LightTextureDesc, NULL, &m_pLightTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating light texture.", L"Error", MB_OK);
	}
	m_pLightTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("LightTexture"), "LightTexture");

	// Create the light render target.
	D3D11_RENDER_TARGET_VIEW_DESC lightRTVDesc;
	ZeroMemory(&lightRTVDesc, sizeof(lightRTVDesc));
	lightRTVDesc.Format = LightTextureDesc.Format;
	lightRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	lightRTVDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(m_pLightTexture, &lightRTVDesc, &m_pLightRTV);
	if (FAILED(hr))
		MessageBox(NULL, L"Error creating light RT.", L"Error", MB_OK);
	m_pLightRTV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("LightRT"), "LightRT");

	//create light shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC lightSRVDesc;
	lightSRVDesc.Format = LightTextureDesc.Format;
	lightSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	lightSRVDesc.Texture2D.MostDetailedMip = 0;
	lightSRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pLightTexture, &lightSRVDesc, &m_pLightSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating light SRV.", L"Error", MB_OK);
	}
	m_pLightSRV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("LightSRV"), "LightSRV");

	//Final.

	//Create the final texture
	D3D11_TEXTURE2D_DESC FinalTextureDesc;
	ZeroMemory(&FinalTextureDesc, sizeof(FinalTextureDesc));
	FinalTextureDesc.Width = width;
	FinalTextureDesc.Height = height;
	FinalTextureDesc.MipLevels = 1;
	FinalTextureDesc.ArraySize = 1;
	FinalTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	FinalTextureDesc.SampleDesc.Count = 1;
	FinalTextureDesc.SampleDesc.Quality = 0;
	FinalTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	FinalTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	FinalTextureDesc.CPUAccessFlags = 0;

	hr = pDevice->CreateTexture2D(&FinalTextureDesc,NULL, &m_pFinalTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating final texture.", L"Error", MB_OK);
	}
	m_pFinalTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("FinalTexture"), "FinalTexture");


	// Create the final render target
	D3D11_RENDER_TARGET_VIEW_DESC finalRTVDesc;
	finalRTVDesc.Format = FinalTextureDesc.Format;
	finalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	finalRTVDesc.Texture2D.MipSlice = 0;
	
	hr = pDevice->CreateRenderTargetView(m_pFinalTexture, &finalRTVDesc, &m_pFinalRTV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating final RT.", L"Error", MB_OK);
	}
	m_pFinalRTV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("FinalRT"), "FinalRT");


	//create the final shader resouce view
	D3D11_SHADER_RESOURCE_VIEW_DESC finalSRVDesc;
	finalSRVDesc.Format = FinalTextureDesc.Format;
	finalSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	finalSRVDesc.Texture2D.MostDetailedMip = 0;
	finalSRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pFinalTexture, &finalSRVDesc, &m_pFinalSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating final SRV.", L"Error", MB_OK);
	}
	m_pFinalSRV->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen("FinalSRV"), "FinalSRV");

	m_pGBufferRenderer = new GBufferRenderer();
	m_pDirectionalLightingRenderer = new LightingRenderer();
	m_pCombineFinalRenderer = new CombineFinalRenderer();

	m_pDrawTexturePixelShader_UnUsed = CommonShaderResourceManager::GetInstance()->CreatePixelShader(L"DrawTexturePixelShader.hlsl", "main");

	m_pCamera = new Camera();
	m_pCamera->SetScreenSolution(CommonShaderResourceManager::GetInstance()->GetScreenSolution());

	m_pScene = new Scene(pDevice);

	OutputDebugString(L"Deferred renderer initialized\n");
}


DeferredRenderer::~DeferredRenderer(void)
{
	delete m_pGBufferRenderer;
	delete m_pDirectionalLightingRenderer;
	delete m_pCombineFinalRenderer;

	delete m_pCamera;
	delete m_pScene;

	if (m_pAlbedoTexture)
	{
		m_pAlbedoTexture->Release();
		m_pAlbedoTexture = nullptr;
	}

	if (m_pAlbedoRTV)
	{
		m_pAlbedoRTV->Release();
		m_pAlbedoRTV = nullptr;
	}

	if (m_pAlbedoSRV)
	{
		m_pAlbedoSRV->Release();
		m_pAlbedoSRV = nullptr;
	}

	if (m_pNormalTexture)
	{
		m_pNormalTexture->Release();
		m_pNormalTexture = nullptr;
	}

	if (m_pNormalRTV)
	{
		m_pNormalRTV->Release();
		m_pNormalRTV = nullptr;
	}

	if (m_pNormalSRV)
	{
		m_pNormalSRV->Release();
		m_pNormalSRV = nullptr;
	}

	if (m_pDepthTexture)
	{
		m_pDepthTexture->Release();
		m_pDepthTexture = nullptr;
	}

	if (m_pDepthRTV)
	{
		m_pDepthRTV->Release();
		m_pDepthRTV = nullptr;
	}

	if (m_pDepthSRV)
	{
		m_pDepthSRV->Release();
		m_pDepthSRV = nullptr;
	}

	if (m_pLightTexture)
	{
		m_pLightTexture->Release();
		m_pLightTexture = nullptr;
	}

	if (m_pLightRTV)
	{
		m_pLightRTV->Release();
		m_pLightRTV = nullptr;
	}

	if (m_pLightSRV)
	{
		m_pLightSRV->Release();
		m_pLightSRV = nullptr;
	}

	if (m_pFinalTexture)
	{
		m_pFinalTexture->Release();
		m_pFinalTexture = nullptr;
	}

	if (m_pFinalRTV)
	{
		m_pFinalRTV->Release();
		m_pFinalRTV = nullptr;
	}

	if (m_pFinalSRV)
	{
		m_pFinalSRV->Release();
		m_pFinalSRV = nullptr;
	}


	if (m_pDrawTexturePixelShader_UnUsed)
	{
		m_pDrawTexturePixelShader_UnUsed->Release();
		m_pDrawTexturePixelShader_UnUsed = nullptr;
	}
	if (m_pLinearSampler)
	{
		m_pLinearSampler->Release();
		m_pLinearSampler = nullptr;
	}
}

void DeferredRenderer::Update()
{
	InputManager::Instance()->Update();
	m_pCamera->Update();
}

void DeferredRenderer::Render()
{
	ID3D11ShaderResourceView* NullSRVs[3] = { nullptr,nullptr,nullptr };

	m_pGBufferRenderer->Render(m_pAlbedoRTV, m_pNormalRTV, m_pDepthRTV, m_pCamera, m_pScene);

	m_pDirectionalLightingRenderer->Render(m_pLightRTV, m_pAlbedoSRV, m_pNormalSRV, m_pDepthSRV, m_pCamera);

	m_pCombineFinalRenderer->Render(m_pFinalRTV, m_pAlbedoSRV, m_pLightSRV);


	//Draw render targets
	ID3D11RenderTargetView* backBufferRTV = CommonShaderResourceManager::GetInstance()->GetBackBufferRenderTarget();

	CommonShaderResourceManager::GetInstance()->GetImmediateContext()->OMSetRenderTargets(1, &backBufferRTV, 0);

	float ClearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

	CommonShaderResourceManager::GetInstance()->GetImmediateContext()->ClearRenderTargetView(backBufferRTV, ClearColor);

	QuadRenderer::GetInstance()->SetSamplerState(0, 1, CommonShaderResourceManager::GetInstance()->GetSamplerLinear());
	QuadRenderer::GetInstance()->SetShaderResource(0, m_pFinalSRV);
	QuadRenderer::GetInstance()->SetPixelShader(m_pDrawTexturePixelShader_UnUsed);
	QuadRenderer::GetInstance()->Render();


	//Unbind
	CommonShaderResourceManager::GetInstance()->GetImmediateContext()->PSSetShaderResources(0, 3, NullSRVs);

	CommonShaderResourceManager::GetInstance()->Present();

}
