#include "Stdafx.h"
#include "FrustumClass.h"

FrustumClass::FrustumClass() {}
FrustumClass::FrustumClass(const FrustumClass& other) {}
FrustumClass::~FrustumClass() {}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	// ���� ����� XMFLOAT4X4 Ÿ������ ����
	XMFLOAT4X4 pMatrix;
	XMStoreFloat4x4(&pMatrix, projectionMatrix);	// XMMATRIX�� XMFLOAT4X4�� �����ϴ� �Լ�

	// ����ü���� z���� �ּ� �Ÿ� ���
	float zMinimum = -pMatrix._43 / pMatrix._33;	// pMatrix._ab : ����� (a, b)�� �ִ� ��
	float r = screenDepth / (screenDepth - zMinimum);
	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	projectionMatrix = XMLoadFloat4x4(&pMatrix);	// XMFLOAT4X4�� XMMATRIX�� �ε��ϴ� �Լ�

	// �þ� ��İ� ������Ʈ�� ������ķ� ����ü ��� ����
	XMMATRIX frustumMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// ������� ����ü ��ĵ� XMFLOAT4X4 Ÿ������ ����
	XMFLOAT4X4 fMatrix;
	XMStoreFloat4x4(&fMatrix, frustumMatrix);

	float x, y, z, w;

	// ����ü�� ����� ��� ���
	x = static_cast<float>(fMatrix._14 + fMatrix._13);
	y = static_cast<float>(fMatrix._24 + fMatrix._23);
	z = static_cast<float>(fMatrix._34 + fMatrix._33);
	w = static_cast<float>(fMatrix._44 + fMatrix._43);
	m_planes[0] = XMVectorSet(x, y, z, w);	// ��麤��
	m_planes[0] = XMPlaneNormalize(m_planes[0]);

	// ����ü�� �� ��� ���
	x = static_cast<float>(fMatrix._14 - fMatrix._13);
	y = static_cast<float>(fMatrix._24 - fMatrix._23);
	z = static_cast<float>(fMatrix._34 - fMatrix._33);
	w = static_cast<float>(fMatrix._44 - fMatrix._43);
	m_planes[1] = XMVectorSet(x, y, z, w);	// ��麤��
	m_planes[1] = XMPlaneNormalize(m_planes[1]);

	// ����ü�� ���� ��� ���
	x = static_cast<float>(fMatrix._14 + fMatrix._11);
	y = static_cast<float>(fMatrix._24 + fMatrix._21);
	z = static_cast<float>(fMatrix._34 + fMatrix._31);
	w = static_cast<float>(fMatrix._44 + fMatrix._41);
	m_planes[2] = XMVectorSet(x, y, z, w);	// ��麤��
	m_planes[2] = XMPlaneNormalize(m_planes[2]);

	// ����ü�� ������ ��� ���
	x = static_cast<float>(fMatrix._14 - fMatrix._11);
	y = static_cast<float>(fMatrix._24 - fMatrix._21);
	z = static_cast<float>(fMatrix._34 - fMatrix._31);
	w = static_cast<float>(fMatrix._44 - fMatrix._41);
	m_planes[3] = XMVectorSet(x, y, z, w);	// ��麤��
	m_planes[3] = XMPlaneNormalize(m_planes[3]);

	// ����ü�� �� ��� ���
	x = static_cast<float>(fMatrix._14 - fMatrix._12);
	y = static_cast<float>(fMatrix._24 - fMatrix._22);
	z = static_cast<float>(fMatrix._34 - fMatrix._32);
	w = static_cast<float>(fMatrix._44 - fMatrix._42);
	m_planes[4] = XMVectorSet(x, y, z, w);	// ��麤��
	m_planes[4] = XMPlaneNormalize(m_planes[4]);

	// ����ü�� �Ʒ� ��� ���
	x = static_cast<float>(fMatrix._14 + fMatrix._12);
	y = static_cast<float>(fMatrix._24 + fMatrix._22);
	z = static_cast<float>(fMatrix._34 + fMatrix._32);
	w = static_cast<float>(fMatrix._44 + fMatrix._42);
	m_planes[5] = XMVectorSet(x, y, z, w);	// ��麤��
	m_planes[5] = XMPlaneNormalize(m_planes[5]);
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	for (int i = 0; i < 6; i++) {
		// ����ü�� 6�� ��� �ȿ� ���� ��� �ִ��� Ȯ��
		// XMPlaneDotCoord(): �Է� ���� ���� ������ ���� ���
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)	return false;
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++) {
		// ����ü�� 6�� ��� �ȿ� ���簢 ť���� 8�� �������� ��� �ִ��� Ȯ��
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
		// ����ü�� 6�� ��� �ȿ� ���� ��� �ִ��� Ȯ��
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f))) < -radius)	return false;
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	for (int i = 0; i < 6; i++) {
		// ����ü�� 6�� ��� �ȿ� �Ϲ����� ť���� 8�� �������� ��� �ִ��� Ȯ��
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