#pragma once

#include <DirectXMath.h>

struct BaseConstantBuffer
{
	DirectX::XMFLOAT4X4 m_World;
	DirectX::XMFLOAT4X4 m_View;
	DirectX::XMFLOAT4X4 m_Projection;
};

__declspec(align(16))
struct LightingConstantBuffer
{
	DirectX::XMFLOAT4 m_LightDirection;
	DirectX::XMFLOAT4 m_Color;
	DirectX::XMFLOAT3 m_CameraPosition;
	DirectX::XMFLOAT4X4 m_InvertViewMatrix;
};
