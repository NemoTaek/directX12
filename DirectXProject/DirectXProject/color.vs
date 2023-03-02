// HLSL 셰이더
// 색상이 있는 삼각형을 그리기 위한 튜토리얼 HLSL 파일
// 정점 셰이더

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	// world, view, projection 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀셰이더가 사용할 입력 색상 저장
	output.color = input.color;

	return output;
}