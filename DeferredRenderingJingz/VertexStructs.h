#pragma once
#include "DirectXMath.h"

struct VertexPositionTexture
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TexCoord;
};

struct VertexPositionTextureNormal
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Normal;
};
