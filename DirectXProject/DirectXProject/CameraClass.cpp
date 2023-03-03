#include "Stdafx.h"
#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
CameraClass::CameraClass(const CameraClass& other) {}
CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition() { return m_position; }

XMFLOAT3 CameraClass::GetRotation() { return m_rotation; }

// ī�޶� ��ġ�� ȸ�� ���¿� ����� �� ����� �����ϴ� �޼ҵ�
void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float xRotation, yRotation, zRotation;	// x, y, z���� ���� ȸ�� �ݰ�, ������ roll, pitch, yaw��� �ϳ�����
	XMMATRIX rotationMatrix;

	// ������ ����Ű�� ����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// ī�޶� ��ġ ����
	position = m_position;
	positionVector = XMLoadFloat3(&position);

	// ī�޶� ã���ִ� ����
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	xRotation = m_rotation.x * 0.0174532925f;	// 0.0174532925 = Pi/180 = 1��
	yRotation = m_rotation.y * 0.0174532925f;
	zRotation = m_rotation.z * 0.0174532925f;

	// ȸ����� ����
	rotationMatrix = XMMatrixRotationRollPitchYaw(xRotation, yRotation, zRotation);

	// lookAt �� up ���͸� ȸ����ķ� �����Ͽ� �䰡 �������� �ùٸ��� ȸ���ϵ��� �Ѵ�
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// ȸ���� ī�޶� ��ġ�� �� ��ġ�� ��ȯ
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 3���� ������Ʈ�� ���Ϳ��� �� ����� �����
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

// ���̴����� �������� ����� �� �ֵ��� ī�޶� ��ü�� �� ����� �޾ƿ��� �޼ҵ�
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; }