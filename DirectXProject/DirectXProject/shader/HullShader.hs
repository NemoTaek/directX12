cbuffer TessellationBuffer
{
	float tessellationAmount;
	float3 padding;
};

struct HullInputType
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct ConstantOutputType
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
	float3 position : POSITION;
	float4 color : COLOR;
};

// 패치 상수 함수
// 전체 패치에서 값이 변하지 않는 데이터를 설정, 계산하는데 사용
// 여기서는 삼각형의 세 꼭지점과 중심점을 위한 테셀레이션 계수를 설정
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	// 삼각형의 세 모서리에 대한 테셀레이션 계수 설정
	output.edges[0] = tessellationAmount;
	output.edges[1] = tessellationAmount;
	output.edges[2] = tessellationAmount;

	// 삼각형 중심점에 대한 테셀레이션 계수 설정
	output.inside = tessellationAmount;

	return output;
}

// 헐 셰이더
// 출력되는 각 제어점마다 패치 상수함수와 헐 셰이더를 호출
// 여기서는 계산된 색상과 제어점을 도메인 셰이더에 전달
[domain("tri")]										// 패치의 종류: 삼각형
[partitioning("integer")]							// 테셀레이션 분할 종류: 정수
[outputtopology("triangle_cw")]						// 테셀레이터가 출력할 기본 도형의 종류: 삼각형
[outputcontrolpoints(3)]							// 출력할 제어점의 수: 3개
[patchconstantfunc("ColorPatchConstantFunction")]	// 테셀레이터에 전달할 때 패치 상수 데이터를 설정하기 위해 호출할 상수 함수의 이름
HullOutputType TessellationHullShader(InputPatch<HullInputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOutputType output;

	// 현재 제어점의 위치를 출력 위치로 사용
	output.position = patch[pointId].position;

	// 입력 색상을 출력 색상으로 사용
	output.color = patch[pointId].color;

	return output;
}