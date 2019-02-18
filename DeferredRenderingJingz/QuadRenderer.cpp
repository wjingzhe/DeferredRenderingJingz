#include "QuadRenderer.h"
#include "CommonShaderResourceManager.h"
#include "VertexStructs.h"

using namespace DirectX;

QuadRenderer* QuadRenderer::s_pInstance = nullptr;

QuadRenderer::QuadRenderer(void)
{
	// Compile the quad shader
	ID3DBlob* pQuadVertexShaderBlob = nullptr;
	HRESULT hr = CommonShaderResourceManager::GetInstance()->CompileShaderFromFile(L"QuadVertexShader.hlsl", "main", "vs_5_0", &pQuadVertexShaderBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The Quad FX file cannot be compiled.", L"Error", MB_OK);
	}

	//Create the quad vertex shader
	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateVertexShader(pQuadVertexShaderBlob->GetBufferPointer(),
		pQuadVertexShaderBlob->GetBufferSize(), NULL, &m_pQuadVertexShader);

	if (FAILED(hr))
	{
		pQuadVertexShaderBlob->Release();
		MessageBox(NULL, L"The Quad shader cannot be created.", L"Error", MB_OK);
	}

	//Define the quad input layout.
	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	UINT quadNumElements = ARRAYSIZE(quadLayout);

	//Create the quad input layout

	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateInputLayout(quadLayout, quadNumElements, pQuadVertexShaderBlob->GetBufferPointer(), pQuadVertexShaderBlob->GetBufferSize(), &m_pQuadVertexLayout);

	pQuadVertexShaderBlob->Release();

	if (FAILED(hr))
	{
		MessageBox(NULL, L"The Quad input layout cannot be created.", L"Error", MB_OK);
	}

	//Create quad vertex buffer.
	VertexPositionTexture quadVertices[] =
	{
		{ XMFLOAT3(1.0f,-1.0f,0.0f),XMFLOAT2(1.0f,1.0f) },
		{ XMFLOAT3(-1.0f,-1.0f,0.0f),XMFLOAT2(0.0f,1.0f) },
		{ XMFLOAT3(-1.0f,1.0f,0.0f),XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(1.0f,1.0f,0.0f),XMFLOAT2(1.0f,0.0f) },
	};

	D3D11_BUFFER_DESC bdQuad;
	ZeroMemory(&bdQuad, sizeof(bdQuad));
	bdQuad.Usage = D3D11_USAGE_DEFAULT;
	bdQuad.ByteWidth = sizeof(VertexPositionTexture) * 4;
	bdQuad.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdQuad.CPUAccessFlags = 0;
	bdQuad.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitQuadData;
	ZeroMemory(&InitQuadData, sizeof(InitQuadData));
	InitQuadData.pSysMem = quadVertices;
	InitQuadData.SysMemPitch = 0;
	InitQuadData.SysMemSlicePitch = 0;
	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateBuffer(&bdQuad, &InitQuadData, &m_pQuadVertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The Quad buffer cannot be created.", L"Error", MB_OK);
	}

	// Create quad index buffer.
	WORD quadIndices[] =
	{
		0,1,2,
		2,3,0
	};

	bdQuad.Usage = D3D11_USAGE_DEFAULT;
	bdQuad.ByteWidth = sizeof(WORD) * 6;
	bdQuad.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdQuad.CPUAccessFlags = 0;
	InitQuadData.pSysMem = quadIndices;
	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateBuffer(&bdQuad, &InitQuadData, &m_pQuadIndexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The Quad buffer cannot be created.", L"Error", MB_OK);
	}

	OutputDebugString(L"QuadRenderer Initialized\n");
}

QuadRenderer::~QuadRenderer(void)
{
	if (m_pQuadVertexLayout)
	{
		m_pQuadVertexLayout->Release();
		m_pQuadVertexLayout = nullptr;
	}

	if (m_pQuadVertexShader)
	{
		m_pQuadVertexShader->Release();
		m_pQuadVertexShader = nullptr;
	}

	if (m_pQuadVertexBuffer)
	{
		m_pQuadVertexBuffer->Release();
		m_pQuadVertexBuffer = nullptr;
	}

	if (m_pQuadIndexBuffer)
	{
		m_pQuadIndexBuffer->Release();
		m_pQuadIndexBuffer = nullptr;
	}

	if (m_pQuadPixelShader)
	{
		m_pQuadPixelShader->Release();
		m_pQuadPixelShader = nullptr;
	}

}

QuadRenderer * QuadRenderer::GetInstance()
{
	if (!s_pInstance)
	{
		s_pInstance = new QuadRenderer();
	}
	return s_pInstance;
}

void QuadRenderer::Render()
{
	auto pImmediateContext = CommonShaderResourceManager::GetInstance()->GetImmediateContext();
	pImmediateContext->IASetInputLayout(m_pQuadVertexLayout);

	// Set primitive topology
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set vertex buffer
	UINT quadStride = sizeof(VertexPositionTexture);
	UINT quadOffset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &m_pQuadVertexBuffer, &quadStride, &quadOffset);
	pImmediateContext->IASetIndexBuffer(m_pQuadIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->VSSetShader(m_pQuadVertexShader, NULL, 0);
	pImmediateContext->PSSetShader(m_pQuadPixelShader, NULL, 0);

	pImmediateContext->DrawIndexed(6, 0, 0);

}

void QuadRenderer::SetSamplerState(UINT StartSlot, UINT NumSampler, ID3D11SamplerState * pSamplerState)
{
	auto pImmediateContext = CommonShaderResourceManager::GetInstance()->GetImmediateContext();
	pImmediateContext->PSSetSamplers(StartSlot, NumSampler, &pSamplerState);
}

void QuadRenderer::SetShaderResource(UINT StartSlot, ID3D11ShaderResourceView * pSRV)
{
	auto pImmediateContext = CommonShaderResourceManager::GetInstance()->GetImmediateContext();
	pImmediateContext->PSSetShaderResources(StartSlot, 1, &pSRV);
}

void QuadRenderer::SetPixelShader(ID3D11PixelShader * pPixelShader)
{
	m_pQuadPixelShader = pPixelShader;

}

void QuadRenderer::SetConstantBuffer(int Slot, ID3D11Buffer * pConstantBuffer)
{
	auto pImmediateContext = CommonShaderResourceManager::GetInstance()->GetImmediateContext();
	pImmediateContext->PSSetConstantBuffers(Slot, 1, &pConstantBuffer);
}

void QuadRenderer::UpdateSubresource(ID3D11Resource * pResource, const void * pSrcData)
{
	auto pImmediateContext = CommonShaderResourceManager::GetInstance()->GetImmediateContext();
	pImmediateContext->UpdateSubresource(pResource, 0, NULL, pSrcData, 0, 0);
}
