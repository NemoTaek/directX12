#include "Stdafx.h"
#include "ModelListClass.h"

#include <time.h>

ModelListClass::ModelListClass() {}
ModelListClass::ModelListClass(const ModelListClass& other) {}
ModelListClass::~ModelListClass() {}

bool ModelListClass::Initialize(int numModels)
{
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;

	// 모델 수 저장
	m_modelCount = numModels;

	// 모델 정보 리스트 생성
	m_modelInfoList = new ModelInfoType[m_modelCount];
	if (!m_modelInfoList)	return false;

	// 랜덤 시드 생성
	srand(static_cast<unsigned int>(time(NULL)));

	// 랜덤으로 모델 색과 위치 생성
	for (int i = 0; i < m_modelCount; i++) {
		red = static_cast<float>(rand()) / RAND_MAX;
		green = static_cast<float>(rand()) / RAND_MAX;
		blue = static_cast<float>(rand()) / RAND_MAX;

		m_modelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);
		m_modelInfoList[i].positionX = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 10.0f;
		m_modelInfoList[i].positionY = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 10.0f;
		m_modelInfoList[i].positionZ = (((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelListClass::Shutdown()
{
	// 모델 정보 리스트 해제
	if (m_modelInfoList) {
		delete[] m_modelInfoList;
		m_modelInfoList = 0;
	}
}

int ModelListClass::GetModelCount() { return m_modelCount; }

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color)
{
	positionX = m_modelInfoList[index].positionX;
	positionY = m_modelInfoList[index].positionY;
	positionZ = m_modelInfoList[index].positionZ;
	color = m_modelInfoList[index].color;
}