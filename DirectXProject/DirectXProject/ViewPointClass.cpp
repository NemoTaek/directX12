#include "Stdafx.h"
#include "ViewPointClass.h"

ViewPointClass::ViewPointClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
ViewPointClass::ViewPointClass(const ViewPointClass& other) {}
ViewPointClass::~ViewPointClass() {}

void ViewPointClass::SetPosition(float x, float y, float z) { m_position = XMFLOAT3(x, y, z); }

void ViewPointClass::SetLookAt(float x, float y, float z) { m_lookAt = XMFLOAT3(x, y, z); }

void ViewPointClass::SetProjectionParameters(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
}

void ViewPointClass::GenerateViewMatrix()
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR upVector = XMLoadFloat3(&up);
	XMVECTOR positionVector = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVector = XMLoadFloat3(&m_lookAt);
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void ViewPointClass::GenerateProjectionMatrix()
{
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}

void ViewPointClass::GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; }

void ViewPointClass::GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = m_projectionMatrix; }