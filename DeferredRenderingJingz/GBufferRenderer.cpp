#include "GBufferRenderer.h"
#include "CommonShaderResourceManager.h"
#include "QuadRenderer.h"
using namespace DirectX;

struct InstancedData
{
	XMMATRIX World;
};

GBufferRenderer::GBufferRenderer(void)
{
	ID3DBlob* renderVertexShaderBlob = NULL;
	m_pVertexInputLayout = NULL;

	m_pRenderGBufferVertexShader = CommonShaderResourceManager::GetInstance()->CreateVertexShader(L"RenderGBufferVertexShader.hlsl", "MainVS",&renderVertexShaderBlob);
	m_pRenderGBufferPixelShader = CommonShaderResourceManager::GetInstance()->CreatePixelShader(L"RenderGBufferPixelShader.hlsl", "MainPS");


	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BaseConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The constant buffer cannot be created.", L"Error", MB_OK);
	}

	{
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(InstancedData) * 64*10*64;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateBuffer(&vbd, nullptr, &m_pInstancedVertexBuffer);
	}


	

	D3D11_INPUT_ELEMENT_DESC modelLayout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0 },

		{ "WORLD",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA ,1 },
		{ "WORLD",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA ,1 },
		{ "WORLD",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA ,1 },
		{ "WORLD",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA ,1 },
	};

	UINT modelNumElements = ARRAYSIZE(modelLayout);

	// Create the quad input layout

	hr = CommonShaderResourceManager::GetInstance()->GetDevice()->CreateInputLayout(modelLayout, modelNumElements, renderVertexShaderBlob->GetBufferPointer(), renderVertexShaderBlob->GetBufferSize(), &m_pVertexInputLayout);

	if (renderVertexShaderBlob)
	{
		renderVertexShaderBlob->Release();
		renderVertexShaderBlob = nullptr;
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"The Quad input layout cannot be created.", L"Error", MB_OK);
	}
}

GBufferRenderer::~GBufferRenderer(void)
{

	if (m_pRenderGBufferVertexShader)
	{
		m_pRenderGBufferVertexShader->Release();
		m_pRenderGBufferVertexShader = nullptr;
	}

	if (m_pRenderGBufferPixelShader)
	{
		m_pRenderGBufferPixelShader->Release();
		m_pRenderGBufferPixelShader = nullptr;
	}

	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = nullptr;
	}

	if (m_pVertexInputLayout)
	{
		m_pVertexInputLayout->Release();
		m_pVertexInputLayout = nullptr;
	}

}

void GBufferRenderer::Render(ID3D11RenderTargetView * pAlbedo, ID3D11RenderTargetView * pNormal, ID3D11RenderTargetView * pDepth, Camera * pCamera, Scene * pScene)
{

	// Update our time

	static float t = 0.0f;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeCur == 0)
	{
		dwTimeStart = dwTimeCur;
	}
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	QuadRenderer *pQuadRenderer = QuadRenderer::GetInstance();
	auto pCommonShaderResourceManager = CommonShaderResourceManager::GetInstance();

	ClearRenderTargetViews(pAlbedo, pNormal, pDepth);

	ID3D11RenderTargetView* RTViews[3] = { pAlbedo,pNormal,pDepth };

	pCommonShaderResourceManager->GetImmediateContext()->OMSetRenderTargets(3, RTViews, pCommonShaderResourceManager->GetDepthStencil());
	pCommonShaderResourceManager->ClearDepthStencilView();


	// Set primitive topology
	pCommonShaderResourceManager->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pCommonShaderResourceManager->GetImmediateContext()->VSSetShader(m_pRenderGBufferVertexShader, NULL, 0);
	pCommonShaderResourceManager->GetImmediateContext()->PSSetShader(m_pRenderGBufferPixelShader, NULL, 0);
	pCommonShaderResourceManager->GetImmediateContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pCommonShaderResourceManager->GetImmediateContext()->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);


	UINT strides[2] = { sizeof(VertexPositionTextureNormal) ,sizeof(InstancedData) };
	UINT offsets[2] = { 0,0 };

	ID3D11Buffer* vertexBuffer = pScene->GetModels()[0]->GetVertexBuffer();
	ID3D11Buffer* inderBuffer = pScene->GetModels()[0]->GetIndexBuffer();


	pCommonShaderResourceManager->GetImmediateContext()->IASetIndexBuffer(inderBuffer, DXGI_FORMAT_R16_UINT, 0);

	pCommonShaderResourceManager->GetImmediateContext()->IASetInputLayout(m_pVertexInputLayout);


	BaseConstantBuffer ConstantBuffer;
	ConstantBuffer.m_View = pCamera->GetViewMatrix();
	ConstantBuffer.m_Projection = pCamera->GetProjectionMatrix();
	pCommonShaderResourceManager->GetImmediateContext()->UpdateSubresource(m_pConstantBuffer, 0, NULL, &ConstantBuffer, 0, 0);



	D3D11_MAPPED_SUBRESOURCE mappedData;
	pCommonShaderResourceManager->GetImmediateContext()->Map(m_pInstancedVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	InstancedData * pData = reinterpret_cast<InstancedData*>(mappedData.pData);

	int i = 0;
	for (int x = 0;x < 64;++x)
	{
		for (int y = 0;y<10;++y)
		{
			for (int z = 0;z<64;++z)
			{
				if (pScene->m_Voxels[x][y][z].State != Active)
				{
					pData[i].World = XMMatrixTranspose(XMMatrixTranslation(32 - x*2.0f, -y*2.0f, 32 - z*2.0f));
					++i;
				}
			}
		}
	}
	pCommonShaderResourceManager->GetImmediateContext()->Unmap(m_pInstancedVertexBuffer, 0);

	//jingz ºËÐÄ´úÂë
	ID3D11Buffer * vbs[2] = { vertexBuffer,m_pInstancedVertexBuffer };
	pCommonShaderResourceManager->GetImmediateContext()->IASetVertexBuffers(0, 2, vbs, strides, offsets);

	pCommonShaderResourceManager->GetImmediateContext()->DrawIndexedInstanced(12 * 3, i, 0, 0, 0);


}

void GBufferRenderer::ClearRenderTargetViews(ID3D11RenderTargetView * pAlbedo, ID3D11RenderTargetView * pNormal, ID3D11RenderTargetView * pDepth)
{
	auto pCommonShaderResourceManager = CommonShaderResourceManager::GetInstance();
	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
	pCommonShaderResourceManager->GetImmediateContext()->ClearRenderTargetView(pAlbedo, clearColor);

	clearColor[0] = clearColor[1] = clearColor[2] = 0.5f;
	clearColor[3] = 0.0f;
	pCommonShaderResourceManager->GetImmediateContext()->ClearRenderTargetView(pNormal, clearColor);

	clearColor[0] = clearColor[1] = clearColor[2] = clearColor[3] = 1.0f;
	pCommonShaderResourceManager->GetImmediateContext()->ClearRenderTargetView(pDepth, clearColor);
}
