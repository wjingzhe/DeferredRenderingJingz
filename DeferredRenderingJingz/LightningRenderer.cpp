#include "LightningRenderer.h"
#include "CommonShaderResourceManager.h"
#include "QuadRenderer.h"
using namespace DirectX;

LightingRenderer::LightingRenderer()
{
	HRESULT hr;

	//Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightingConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The light constant buffer cannot be created.", L"Error", MB_OK);
	}

	m_pLightingPixelShader = CommonShaderResourceManager::GetInstance()->CreatePixelShader(L"DirectionalLightPixelShader.hlsl","main");

	OutputDebugString(L"Lightning render initialized\n");

}

LightingRenderer::~LightingRenderer(void)
{
	if (m_pLightingPixelShader)
	{
		m_pLightingPixelShader->Release();
		m_pLightingPixelShader = nullptr;
	}

	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = nullptr;
	}
}

void LightingRenderer::Render(ID3D11RenderTargetView * pLightMapRT, ID3D11ShaderResourceView * pAlbedo, ID3D11ShaderResourceView * pNormal, ID3D11ShaderResourceView * pDepth, Camera * pCamera)
{
	QuadRenderer * pQuadRender = QuadRenderer::GetInstance();
	CommonShaderResourceManager * pCommonShaderResourceManager = CommonShaderResourceManager::GetInstance();

	pCommonShaderResourceManager->GetImmediateContext()->OMSetRenderTargets(1, &pLightMapRT, 0);
	float ClearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
	pCommonShaderResourceManager->GetImmediateContext()->ClearRenderTargetView(pLightMapRT, ClearColor);

	pQuadRender->SetPixelShader(m_pLightingPixelShader);
	pQuadRender->SetShaderResource(0, pAlbedo);
	pQuadRender->SetShaderResource(1, pNormal);
	pQuadRender->SetShaderResource(2, pDepth);

	// Set RenderStates
	pQuadRender->SetSamplerState(0, 1, pCommonShaderResourceManager->GetSamplerLinear());
	pQuadRender->SetSamplerState(1, 1, pCommonShaderResourceManager->GetSamplerPoint());

	// Set constant buffer.
	LightingConstantBuffer lightningConstantBuffer;
	lightningConstantBuffer.m_LightDirection = XMFLOAT4(0.5f, -0.3f, 0.0f, 1.0f);
	lightningConstantBuffer.m_Color = XMFLOAT4(0.99f, 1.0f, 1.0f, 1.0f);
	lightningConstantBuffer.m_CameraPosition = pCamera->GetPosition();

	lightningConstantBuffer.m_InvertViewMatrix = pCamera->GetInvertViewProjection();

	pQuadRender->SetConstantBuffer(0, m_pConstantBuffer);
	pQuadRender->UpdateSubresource(m_pConstantBuffer, &lightningConstantBuffer);

	pQuadRender->Render();
}
