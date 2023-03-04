cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;

	// float4 color : COLOR;

	// TEXCOORD 시맨틱은 픽셀셰이더에서 텍스쳐 좌표를 나타낸다.
	// 텍스쳐 좌표계에는 (u, v) 좌표로 나타내며 각각 0~1의 크기를 갖고 있다.
	float2 tex : TEXCOORD0;
	
	// 방향 조명
	// 방향벡터와 법선벡터가 필요
	// 방향벡터는 직접 지정, 법선벡터는 정점들에 의해 만들어지는 평면으로 계산
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	// float4 color : COLOR;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

PixelInputType VS(VertexInputType input)
{
	PixelInputType output;

	// 적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	// world, view, projection 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀셰이더가 사용할 입력 색상 저장
	// output.color = input.color;

	// 픽셀셰이더가 사용할 텍스쳐 저장
	output.tex = input.tex;

	// 세계 행렬에 대한 법선벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 법선벡터 정규화
	output.normal = normalize(output.normal);

	return output;
}