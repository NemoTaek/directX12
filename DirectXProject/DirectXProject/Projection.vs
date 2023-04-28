cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewMatrix2;
	matrix projectionMatrix2;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
};

PixelInputType ProjectionVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 투영에 의해 보았을때 정점의 위치 계산
	output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.viewPosition, viewMatrix2);
	output.viewPosition = mul(output.viewPosition, projectionMatrix2);

	// 픽셀 쉐이더의 텍스처 좌표를 저장
	output.tex = input.tex;

	// 세계 행렬에 대한 법선 벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}