#include "CommonShaderResourceManager.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include "WindowHandle.h"

using namespace std;


CommonShaderResourceManager* CommonShaderResourceManager::s_pInstance = nullptr;

CommonShaderResourceManager * CommonShaderResourceManager::GetInstance()
{
	if (!s_pInstance)
	{
		s_pInstance = new CommonShaderResourceManager();
	}
	return s_pInstance;
}

ID3D11Device * CommonShaderResourceManager::GetDevice()
{
	return m_pD3DDevice;
}

ID3D11DeviceContext * CommonShaderResourceManager::GetImmediateContext()
{
	return m_pImmediateContext;
}

ID3D11RenderTargetView * CommonShaderResourceManager::GetBackBufferRenderTarget()
{
	return m_pBackBufferRenderTargetView;
}

ID3D11DepthStencilView * CommonShaderResourceManager::GetDepthStencil()
{
	return m_pDepthStencilView;
}

ID3D11SamplerState * CommonShaderResourceManager::GetSamplerLinear()
{
	return m_pSamplerLinear;
}

ID3D11SamplerState * CommonShaderResourceManager::GetSamplerPoint()
{
	return m_pSamplerPoint;
}

// Helper for comiling shaders with D3D11
HRESULT CommonShaderResourceManager::CompileShaderFromFile(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG)|| defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE","1",NULL,NULL
	};

	hr = D3DCompileFromFile(szFileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob!=NULL)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return hr;
	}

	if (pErrorBlob)
	{
		pErrorBlob->Release();
	}

	OutputDebugString(szFileName);
	OutputDebugString(L"Shader Compiled.\n");


	return S_OK;
}

ID3D11PixelShader * CommonShaderResourceManager::CreatePixelShader(WCHAR * pFileName, LPCSTR pEntryPoint, ID3DBlob * pPixelShaderBlobOut)
{
	ID3D11PixelShader* pixelShader;
	
	pPixelShaderBlobOut = NULL;
	HRESULT hr = CompileShaderFromFile(pFileName, pEntryPoint, "ps_5_0", &pPixelShaderBlobOut);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The pixel shade file cannot be compiled.", L"Error", MB_OK);
		pPixelShaderBlobOut->Release();
		pPixelShaderBlobOut = nullptr;
		return nullptr;
	}

	this->GetDevice()->CreatePixelShader(pPixelShaderBlobOut->GetBufferPointer(), pPixelShaderBlobOut->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr))
	{
		pPixelShaderBlobOut->Release();
		pPixelShaderBlobOut = nullptr;
		MessageBox(NULL, L"The pixel shader cannot be created.", L"Error", MB_OK);
		return nullptr;
	}


	return pixelShader;
}

ID3D11PixelShader * CommonShaderResourceManager::CreatePixelShader(WCHAR * pFileName, LPCSTR pEntryPoint)
{
	ID3D11PixelShader* pixelShader;

	pixelShader = CreatePixelShader(pFileName, pEntryPoint, nullptr);

	return pixelShader;
}

ID3D11VertexShader * CommonShaderResourceManager::CreateVertexShader(WCHAR * pFileName, LPCSTR pEntryPoint, ID3DBlob ** ppVertexShaderBlobOut)
{
	ID3D11VertexShader* vertexShader;
	// Compile the quad shader.
	//ID3DBlob* pVSBlob = NULL;
	(*ppVertexShaderBlobOut) = NULL;
	HRESULT hr = CompileShaderFromFile(pFileName, pEntryPoint, "vs_5_0", ppVertexShaderBlobOut);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The vertex shader file cannot be compiled.", L"Error", MB_OK);
		if (*ppVertexShaderBlobOut)
		{
			(*ppVertexShaderBlobOut)->Release();
			(*ppVertexShaderBlobOut) = nullptr;
		}
	}

	// Create the quad vertex shader
	hr = this->GetDevice()->CreateVertexShader((*ppVertexShaderBlobOut)->GetBufferPointer(),
		(*ppVertexShaderBlobOut)->GetBufferSize(),
		NULL,
		&vertexShader);

	if (FAILED(hr))
	{
		(*ppVertexShaderBlobOut)->Release();
		(*ppVertexShaderBlobOut) = nullptr;
		ppVertexShaderBlobOut = nullptr;
		MessageBox(NULL, L"The vertex shader cannot be created.", L"Error", MB_OK);
		return NULL;
	}

	return vertexShader;
}

void CommonShaderResourceManager::ClearDepthStencilView()
{
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CommonShaderResourceManager::Present()
{
	m_pSwapChain->Present(0, 0);
}

void CommonShaderResourceManager::SetOpaque()
{
}

CommonShaderResourceManager::CommonShaderResourceManager()
{
	HRESULT hr = S_OK;

	RECT rc;

	GetClientRect(WindowHandle::s_hWnd, &rc);
	m_ScreenSolution.SetScreenSize(rc.right - rc.left, rc.bottom - rc.top);

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_ScreenSolution.GetScreenWidth();
	sd.BufferDesc.Height = m_ScreenSolution.GetScreenHeight();
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = WindowHandle::s_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;


	for (UINT driveTypeIndex = 0;driveTypeIndex < numDriverTypes; ++driveTypeIndex)
	{
		m_DriveType = driverTypes[driveTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_DriveType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&sd, &m_pSwapChain, &m_pD3DDevice, &m_FeatureLevel, &m_pImmediateContext);

		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error.", L"Error", MB_OK);
	}

	// Create the back buffer.
	// Create a render target view.
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error.", L"Error", MB_OK);
	}

	hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackBufferRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error.", L"Error", MB_OK);
	}

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = m_ScreenSolution.GetScreenWidth();
	depthTextureDesc.Height = m_ScreenSolution.GetScreenHeight();
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.MiscFlags = 0;
	hr = m_pD3DDevice->CreateTexture2D(&depthTextureDesc, NULL, &m_pDepthStencilTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error.", L"Error", MB_OK);
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = depthTextureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilTexture, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error.", L"Error", MB_OK);
	}



	// Create the linear sample state.
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pD3DDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating linear sampler.", L"Error", MB_OK);
	}

	// Create the linear sample state
	D3D11_SAMPLER_DESC sampPointDesc;
	ZeroMemory(&sampPointDesc, sizeof(sampPointDesc));
	sampPointDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampPointDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampPointDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampPointDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampPointDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampPointDesc.MinLOD = 0;
	sampPointDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pD3DDevice->CreateSamplerState(&sampPointDesc, &m_pSamplerPoint);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating point sampler.", L"Error", MB_OK);
	}

	//Setup the viewPort
	D3D11_VIEWPORT vp;
	vp.Width = m_ScreenSolution.GetScreenWidth();
	vp.Height = m_ScreenSolution.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &vp);


	// Setup the Raster description which will determine how and what polygons will be drawn
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	hr = m_pD3DDevice->CreateRasterizerState(&rasterDesc, &m_pRasterizerState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error creating rasterizer state.", L"Error", MB_OK);
	}

	m_pImmediateContext->RSSetState(m_pRasterizerState);//for default

	OutputDebugString(L"Engine initialized\n");

}

CommonShaderResourceManager::~CommonShaderResourceManager()
{
	m_pD3DDevice->Release();
	m_pImmediateContext->Release();
	m_pSwapChain->Release();
	m_pBackBufferRenderTargetView->Release();
	m_pDepthStencilTexture->Release();
	m_pDepthStencilView->Release();
	m_pRasterizerState->Release();
	m_pSamplerLinear->Release();
	m_pSamplerPoint->Release();
	m_pOpaqueBlendState->Release();
	m_pAlphaBlendState->Release();

}
