#pragma once

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(XMFLOAT3);
	void SetRotation(XMFLOAT3);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);
	
	void GenerateBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX&);
	void RenderBaseViewMatrix();

	void RenderReflection(float);
	void GetReflectionViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_baseViewMatrix;
	XMMATRIX m_reflectionViewMatrix;
};