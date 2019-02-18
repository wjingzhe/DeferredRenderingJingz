#include "Camera.h"
#include "InputManager.h"
#include "DirectXMath.h"
using namespace DirectX;

Camera::Camera(void):m_Position(0.0f,0.0f,-20.0f),
	m_Offset(0.0f,10.0f,0.0f),
	m_Pitch(0.0f), m_Yaw(0.0f),m_Roll(0.0f),
	m_RotationSpeed(1.0f),m_MovementSpeed(1.0f), m_pScreenSolution(nullptr)
{
}

Camera::~Camera(void)
{
	m_pScreenSolution = nullptr;
}

void Camera::Update()
{
	int deltaMouseX = 0;
	int deltaMouseY = 0;

	if (InputManager::Instance()->IsMouseDown(0))
	{
		InputManager::Instance()->GetMouseMove(deltaMouseX, deltaMouseY);
		m_Yaw += deltaMouseX*m_RotationSpeed*XM_PI / 180;
		m_Pitch += deltaMouseY* m_RotationSpeed*XM_PI / 180;
	}

	CXMVECTOR Forward = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	CXMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	CXMVECTOR Right = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f);
	XMVECTOR position = XMLoadFloat3(&m_Position);

	XMVECTOR transformedForward = XMVector3Transform(Forward, rotation);
	XMVector3Normalize(transformedForward);

	XMVECTOR tranformedRight = XMVector3Transform(Right, rotation);
	XMVector3Normalize(tranformedRight);

	if (InputManager::Instance()->IsKeyDown(DIK_W))
	{
		position += transformedForward * m_MovementSpeed;
	}

	if (InputManager::Instance()->IsKeyDown(DIK_S))
	{
		position -= transformedForward * m_MovementSpeed;
	}
	if (InputManager::Instance()->IsKeyDown(DIK_A))
	{
		position -= tranformedRight * m_MovementSpeed;
	}

	if (InputManager::Instance()->IsKeyDown(DIK_D))
	{
		position += tranformedRight * m_MovementSpeed;
	}

	if (InputManager::Instance()->IsKeyDown(DIK_Q))
	{
		position += Up*m_MovementSpeed;
	}

	if (InputManager::Instance()->IsKeyDown(DIK_E))
	{
		position -= Up*m_MovementSpeed;
	}

	XMStoreFloat3(&m_Position, position);

	CXMMATRIX view = XMMatrixTranspose(XMMatrixLookAtLH(position, transformedForward + position, Up));
	XMStoreFloat4x4(&m_ViewMatrix, view);


	//Compute projection matrix
	CXMMATRIX proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, m_pScreenSolution ? m_pScreenSolution->GetScreenRatio() : (16.0f / 9.0f), 0.01f, 10000.0f));
	XMStoreFloat4x4(&m_ProjectionMatrix, proj);

	//Compute projection matrix
	CXMMATRIX invertViewProj = XMMatrixInverse(nullptr, XMMatrixMultiply(view, proj));
	XMStoreFloat4x4(&m_InvertViewProjectionMatrix, invertViewProj);

}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return m_ViewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return m_ProjectionMatrix;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return m_Position;
}

DirectX::XMFLOAT4X4 Camera::GetInvertViewProjection()
{
	return m_InvertViewProjectionMatrix;
}
