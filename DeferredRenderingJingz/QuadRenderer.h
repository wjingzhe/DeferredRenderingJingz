#pragma once

#include <d3d11.h>

class QuadRenderer
{
public:
	static QuadRenderer* GetInstance();
	void Render();

	void SetSamplerState(UINT StartSlot, UINT NumSampler, ID3D11SamplerState* pSamplerState);
	void SetShaderResource(UINT StartSlot, ID3D11ShaderResourceView* pSRV);

	void SetPixelShader(ID3D11PixelShader* pPixelShader);

	void SetConstantBuffer(int Slot, ID3D11Buffer* pConstantBuffer);

	void UpdateSubresource(ID3D11Resource* pResource, const void* pSrcData);

protected:
	QuadRenderer(void);
	~QuadRenderer(void);

	static QuadRenderer* s_pInstance;

	// Full screen quad data
	// General data,common to all fullscreen quads
	ID3D11InputLayout* m_pQuadVertexLayout;
	ID3D11VertexShader* m_pQuadVertexShader;
	ID3D11Buffer* m_pQuadVertexBuffer;
	ID3D11Buffer* m_pQuadIndexBuffer;

	ID3D11PixelShader* m_pQuadPixelShader;

private:
};