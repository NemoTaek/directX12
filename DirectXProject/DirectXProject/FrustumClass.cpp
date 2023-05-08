#include "Stdafx.h"
#include "FrustumClass.h"

FrustumClass::FrustumClass() {}
FrustumClass::FrustumClass(const FrustumClass& other) {}
FrustumClass::~FrustumClass() {}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	// 투영 행렬을 XMFLOAT4X4 타입으로 변경
	XMFLOAT4X4 pMatrix;
	XMStoreFloat4x4(&pMatrix, projectionMatrix);	// XMMATRIX를 XMFLOAT4X4에 저장하는 함수

	// 절두체에서 z축의 최소 거리 계산
	float zMinimum = -pMatrix._43 / pMatrix._33;	// pMatrix._ab : 행렬의 (a, b)에 있는 값
	float r = screenDepth / (screenDepth - zMinimum);
	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	projectionMatrix = XMLoadFloat4x4(&pMatrix);	// XMFLOAT4X4를 XMMATRIX에 로드하는 함수

	// 시야 행렬과 업데이트된 투영행렬로 절두체 행렬 생성
	XMMATRIX frustumMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// 만들어진 절두체 행렬도 XMFLOAT4X4 타입으로 변경
	XMFLOAT4X4 fMatrix;
	XMStoreFloat4x4(&fMatrix, frustumMatrix);

	float x, y, z, w;

	// 절두체의 가까운 평면 계산
	x = static_cast<float>(fMatrix._14 + fMatrix._13);
	y = static_cast<float>(fMatrix._24 + fMatrix._23);
	z = static_cast<float>(fMatrix._34 + fMatrix._33);
	w = static_cast<float>(fMatrix._44 + fMatrix._43);
	m_planes[0] = XMVectorSet(x, y, z, w);	// 평면벡터
	m_planes[0] = XMPlaneNormalize(m_planes[0]);

	// 절두체의 먼 평면 계산
	x = static_cast<float>(fMatrix._14 - fMatrix._13);
	y = static_cast<float>(fMatrix._24 - fMatrix._23);
	z = static_cast<float>(fMatrix._34 - fMatrix._33);
	w = static_cast<float>(fMatrix._44 - fMatrix._43);
	m_planes[1] = XMVectorSet(x, y, z, w);	// 평면벡터
	m_planes[1] = XMPlaneNormalize(m_planes[1]);

	// 절두체의 왼쪽 평면 계산
	x = static_cast<float>(fMatrix._14 + fMatrix._11);
	y = static_cast<float>(fMatrix._24 + fMatrix._21);
	z = static_cast<float>(fMatrix._34 + fMatrix._31);
	w = static_cast<float>(fMatrix._44 + fMatrix._41);
	m_planes[2] = XMVectorSet(x, y, z, w);	// 평면벡터
	m_planes[2] = XMPlaneNormalize(m_planes[2]);

	// 절두체의 오른쪽 평면 계산
	x = static_cast<float>(fMatrix._14 - fMatrix._11);
	y = static_cast<float>(fMatrix._24 - fMatrix._21);
	z = static_cast<float>(fMatrix._34 - fMatrix._31);
	w = static_cast<float>(fMatrix._44 - fMatrix._41);
	m_planes[3] = XMVectorSet(x, y, z, w);	// 평면벡터
	m_planes[3] = XMPlaneNormalize(m_planes[3]);

	// 절두체의 위 평면 계산
	x = static_cast<float>(fMatrix._14 - fMatrix._12);
	y = static_cast<float>(fMatrix._24 - fMatrix._22);
	z = static_cast<float>(fMatrix._34 - fMatrix._32);
	w = static_cast<float>(fMatrix._44 - fMatrix._42);
	m_planes[4] = XMVectorSet(x, y, z, w);	// 평면벡터
	m_planes[4] = XMPlaneNormalize(m_planes[4]);

	// 절두체의 아래 평면 계산
	x = static_cast<float>(fMatrix._14 + fMatrix._12);
	y = static_cast<float>(fMatrix._24 + fMatrix._22);
	z = static_cast<float>(fMatrix._34 + fMatrix._32);
	w = static_cast<float>(fMatrix._44 + fMatrix._42);
	m_planes[5] = XMVectorSet(x, y, z, w);	// 평면벡터
	m_planes[5] = XMPlaneNormalize(m_planes[5]);
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	for (int i = 0; i < 6; i++) {
		// 절두체의 6개 평면 안에 점이 모두 있는지 확인
		// XMPlaneDotCoord(): 입력 평면과 벡터 사이의 내적 계산
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)	return false;
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++) {
		// 절두체의 6개 평면 안에 정사각 큐브의 8개 꼭지점이 모두 있는지 확인
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter - radius, zCenter - radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter - radius, zCenter - radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter + radius, zCenter - radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter + radius, zCenter - radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter - radius, zCenter + radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter - radius, zCenter + radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - radius, yCenter - radius, zCenter + radius, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + radius, yCenter - radius, zCenter + radius, 1.0f))) >= 0.0f)	continue;

		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++) {
		// 절두체의 6개 평면 안에 구가 모두 있는지 확인
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f))) < -radius)	return false;
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	for (int i = 0; i < 6; i++) {
		// 절두체의 6개 평면 안에 일반적인 큐브의 8개 꼭지점이 모두 있는지 확인
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter - zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter - zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter + ySize, zCenter - zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter + ySize, zCenter - zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter + zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter + zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter + zSize, 1.0f))) >= 0.0f)	continue;
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter + zSize, 1.0f))) >= 0.0f)	continue;

		return false;
	}

	return true;
}