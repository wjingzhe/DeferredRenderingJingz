#include "CombineFinalRenderer.h"
#include "CommonShaderResourceManager.h"
#include "QuadRenderer.h"

CombineFinalRenderer::CombineFinalRenderer()
{
	m_pCombinePixelShader = CommonShaderResourceManager::GetInstance()->CreatePixelShader(L"CombineFinalPixelShader.hlsl", "main");
}

CombineFinalRenderer::~CombineFinalRenderer()
{
	delete m_pCombinePixelShader;
}

void CombineFinalRenderer::Render(ID3D11RenderTargetView * pFinalRT, ID3D11ShaderResourceView * pAlbedo, ID3D11ShaderResourceView * pLight)
{
	QuadRenderer* pQuadRender = QuadRenderer::GetInstance();
	auto pCommonShaderResourceManager = CommonShaderResourceManager::GetInstance();

	pCommonShaderResourceManager->GetImmediateContext()->OMSetRenderTargets(1, &pFinalRT, 0);

	pQuadRender->SetPixelShader(m_pCombinePixelShader);
	pQuadRender->SetShaderResource(0, pAlbedo);
	pQuadRender->SetShaderResource(1, pLight);

	// Set RenderStates
	pQuadRender->SetSamplerState(0, 1, pCommonShaderResourceManager->GetSamplerLinear());

	pQuadRender->Render();
}
