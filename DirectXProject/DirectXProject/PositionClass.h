#pragma once

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetPosition(XMFLOAT3);
	void SetRotation(XMFLOAT3);

	void GetPosition(XMFLOAT3&);
	void GetRotation(XMFLOAT3&);

	void SetFrameTime(float);

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveLeft(bool);
	void MoveRight(bool);
	void MoveUpward(bool);
	void MoveDownward(bool);
	void TurnLeft(bool);
	void TurnRight(bool);
	void LookUpward(bool);
	void LookDownward(bool);

private:
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_frameTime = 0.0f;
	float m_leftSpeed = 0.0f;
	float m_rightSpeed = 0.0f;
	float m_forwardSpeed = 0.0f;
	float m_backwardSpeed = 0.0f;
	float m_upwardSpeed = 0.0f;
	float m_downwardSpeed = 0.0f;
	float m_leftTurnSpeed = 0.0f;
	float m_rightTurnSpeed = 0.0f;
	float m_lookUpSpeed = 0.0f;
	float m_lookDownSpeed = 0.0f;
};