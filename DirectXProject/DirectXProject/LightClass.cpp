#include "Stdafx.h"
#include "LightClass.h"
#include <stdio.h>

LightClass::LightClass() {}
LightClass::LightClass(const LightClass& other) {}
LightClass::~LightClass() {}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = XMFLOAT3(x, y, z);
}

void LightClass::GenerateViewMatrix()
{
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR upVector = XMLoadFloat3(&up);
	XMVECTOR positionVector = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVector = XMLoadFloat3(&m_lookAt);
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView = static_cast<float>(XM_PI) / 2.0f;
	float screenAspect = 1.0f;
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}

XMFLOAT4 LightClass::GetAmbientColor() { return m_ambientColor; }

XMFLOAT4 LightClass::GetDiffuseColor() { return m_diffuseColor; }

XMFLOAT3 LightClass::GetDirection() { return m_direction; }

XMFLOAT4 LightClass::GetSpecularColor() { return m_specularColor; }

float LightClass::GetSpecularPower() { return m_specularPower; }

XMFLOAT3 LightClass::GetPosition() { return m_position; }

void LightClass::GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; }

void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = m_projectionMatrix; }