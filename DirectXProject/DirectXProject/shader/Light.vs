#define NUM_LIGHTS 5

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightPositionBuffer
{
	float4 lightPosition[NUM_LIGHTS];
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
	
	float3 lightPos1 : TEXCOORD1;
	float3 lightPos2 : TEXCOORD2;
	float3 lightPos3 : TEXCOORD3;
	float3 lightPos4 : TEXCOORD4;
	float3 lightPos5 : TEXCOORD5;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// 적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	// world, view, projection 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀셰이더가 사용할 텍스쳐 저장
	output.tex = input.tex;

	// 세계 행렬에 대한 법선벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 법선벡터 정규화
	output.normal = normalize(output.normal);

	// 세계 정점 위치 계산
	worldPosition = mul(input.position, worldMatrix);

	// 조명의 위치와 세계 정점 위치를 기반으로 조명 위치 세팅
	output.lightPos1.xyz = lightPosition[0].xyz - worldPosition.xyz;
	output.lightPos2.xyz = lightPosition[1].xyz - worldPosition.xyz;
	output.lightPos3.xyz = lightPosition[2].xyz - worldPosition.xyz;
	output.lightPos4.xyz = lightPosition[3].xyz - worldPosition.xyz;
	output.lightPos5.xyz = lightPosition[4].xyz - worldPosition.xyz;

	// 빛 위치벡터 정규화
	output.lightPos1 = normalize(output.lightPos1);
	output.lightPos2 = normalize(output.lightPos2);
	output.lightPos3 = normalize(output.lightPos3);
	output.lightPos4 = normalize(output.lightPos4);
	output.lightPos5 = normalize(output.lightPos5);

	return output;
}