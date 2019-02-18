#pragma once
#include "DirectXMath.h"
#include "ScreenSolution.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

	void Update();
	void SetScreenSolution(ScreenSolution* pScreenSolution)
	{
		m_pScreenSolution = pScreenSolution;
	}

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT4X4 GetInvertViewProjection();

protected:


private:
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjectionMatrix;
	DirectX::XMFLOAT4X4 m_InvertViewProjectionMatrix;

	DirectX::XMFLOAT3 m_Offset;
	float m_Pitch;
	float m_Yaw;
	float m_Roll;
	//DirectX::XMFLOAT3 m_CameraReference;
	float m_RotationSpeed;
	float m_MovementSpeed;

	ScreenSolution* m_pScreenSolution;
};