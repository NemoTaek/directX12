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

// 카메라 위치와 회전 상태에 기반한 뷰 행렬을 생성하는 메소드
void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float xRotation, yRotation, zRotation;	// x, y, z축을 기준 회전 반경, 각각을 roll, pitch, yaw라고도 하나보다
	XMMATRIX rotationMatrix;

	// 위쪽을 가리키는 벡터
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// 카메라 위치 설정
	position = m_position;
	positionVector = XMLoadFloat3(&position);

	// 카메라가 찾고있는 벡터
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	xRotation = m_rotation.x * 0.0174532925f;	// 0.0174532925 = Pi/180 = 1도
	yRotation = m_rotation.y * 0.0174532925f;
	zRotation = m_rotation.z * 0.0174532925f;

	// 회전행렬 생성
	rotationMatrix = XMMatrixRotationRollPitchYaw(xRotation, yRotation, zRotation);

	// lookAt 및 up 벡터를 회전행렬로 변형하여 뷰가 원점에서 올바르게 회전하도록 한다
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전된 카메라 위치를 뷰 위치로 변환
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 3개의 업데이트된 벡터에서 뷰 행렬을 만든다
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

// 셰이더에서 렌더링에 사용할 수 있도록 카메라 객체의 뷰 행렬을 받아오는 메소드
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; }