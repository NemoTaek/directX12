#include "Stdafx.h"
#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
CameraClass::CameraClass(const CameraClass& other) {}
CameraClass::~CameraClass() {}

void CameraClass::SetPosition(XMFLOAT3 position) { m_position = position; }

void CameraClass::SetRotation(XMFLOAT3 rotation) { m_rotation = rotation; }

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

	// 회전 각 계산
	xRotation = m_rotation.x * 0.0174532925f;	// 0.0174532925 = Pi/180 = 1도
	yRotation = m_rotation.y * 0.0174532925f;
	zRotation = m_rotation.z * 0.0174532925f;

	// 카메라가 찾고있는 벡터
	//lookAt.x = sinf(yRotation) + m_position.x;
	//lookAt.y = m_position.y;
	//lookAt.z = cosf(yRotation) + m_position.z;
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

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

void CameraClass::GetBaseViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_baseViewMatrix; }

void CameraClass::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	
	// 위쪽을 가리키는 벡터
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// 카메라 위치 설정
	position = m_position;
	positionVector = XMLoadFloat3(&position);

	// 회전 각 계산
	float radians = m_rotation.y * 0.0174532925f;

	// 카메라가 찾고있는 벡터
	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = m_position.y;
	lookAt.z = cosf(radians) + m_position.z;

	lookAtVector = XMLoadFloat3(&lookAt);

	// 3개의 업데이트된 벡터에서 뷰 행렬을 만든다
	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::RenderReflection(float height)
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
	position.x = m_position.x;
	position.y = -m_position.y + (height * 2.0f);
	position.z = m_position.z;
	positionVector = XMLoadFloat3(&position);

	// 회전 각 계산
	xRotation = m_rotation.x * 0.0174532925f;	// 0.0174532925 = Pi/180 = 1도
	yRotation = m_rotation.y * 0.0174532925f;
	zRotation = m_rotation.z * 0.0174532925f;

	// 카메라가 찾고있는 벡터
	lookAt.x = sinf(yRotation) + m_position.x;
	lookAt.y = position.y;
	lookAt.z = cosf(yRotation) + m_position.z;
	lookAtVector = XMLoadFloat3(&lookAt);

	//// 회전행렬 생성
	//rotationMatrix = XMMatrixRotationRollPitchYaw(xRotation, yRotation, zRotation);

	//// lookAt 및 up 벡터를 회전행렬로 변형하여 뷰가 원점에서 올바르게 회전하도록 한다
	//lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	//upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//// 회전된 카메라 위치를 뷰 위치로 변환
	//lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 3개의 업데이트된 벡터에서 뷰 행렬을 만든다
	m_reflectionViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

XMMATRIX CameraClass::GetReflectionViewMatrix() { return m_reflectionViewMatrix; }