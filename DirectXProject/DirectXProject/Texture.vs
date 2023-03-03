// 정점셰이더
// 이번에는 색상 대신 텍스쳐를 입혀볼 것이다

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	// TEXCOORD 시맨틱은 픽셀셰이더에서 텍스쳐 좌표를 나타낸다.
	// 텍스쳐 좌표계에는 (u, v) 좌표로 나타내며 각각 0~1의 크기를 갖고 있다.
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	// world, view, projection 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀셰이더가 사용할 입력 색상 저장
	output.tex = input.tex;

	return output;
}