cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	// 조명의 시점에 기초한 2개의 행렬
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightBuffer2
{
	float3 lightPosition;
	float padding;
}

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

	// 조명 시점으로 변환된 정점의 위치
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

PixelInputType ShadowVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// 적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	// world, view, projection 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 광원에 의해 보았을때 정점의 위치 계산
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// 픽셀셰이더가 사용할 텍스쳐 저장
	output.tex = input.tex;

	// 세계 행렬에 대한 법선벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 법선벡터 정규화
	output.normal = normalize(output.normal);

	// 세계 정점 위치 계산
	worldPosition = mul(input.position, worldMatrix);

	// 조명의 위치와 세계 정점 위치를 기반으로 조명 위치 세팅
	output.lightPos = lightPosition.xyz - worldPosition.xyz;

	// 빛 위치벡터 정규화
	output.lightPos = normalize(output.lightPos);

	return output;
}