#pragma once

#include "VertexStructs.h"
#include <DirectXMath.h>
#include <d3d11.h>

class Model
{
public:
	Model(ID3D11Device* pDevice);
	~Model(void);

	ID3D11Buffer* GetVertexBuffer()
	{
		return m_pVertexBuffer;
	}

	ID3D11Buffer* GetIndexBuffer()
	{
		return m_pIndexBuffer;
	}


private:

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;


};
