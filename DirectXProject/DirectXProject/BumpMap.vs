// Globals

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 정반사 계산을 위한 카메라 위치 정보
cbuffer CameraBuffer
{
	float3 cameraPosition;
}


// Typedefs

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;		// z축(탄젠트 공간에서의 법선이 되는 공간)
	float3 tangent : TANGENT;	// x축(uv 좌표계에서는 u좌표)
	float3 binormal : BINORMAL;	// y축(uv 좌표계에서는 v좌표)
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;		// z축(탄젠트 공간에서의 법선이 되는 공간)
	float3 tangent : TANGENT;	// x축(uv 좌표계에서는 u좌표)
	float3 binormal : BINORMAL;	// y축(uv 좌표계에서는 v좌표)
	float3 viewDirection : TEXCOORD1;	// 반사광 연산에 필요
};


// Vertex Shader

PixelInputType BumpMapVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 세계 행렬에 대해서만 각 벡터를 계산한 다음 최종 값을 정규화한다.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	// 세계 정점 위치 계산
	worldPosition = mul(input.position, worldMatrix);

	// 카메라 위치와 세계 정점 위치를 기준으로 시야 방향 계산
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// 시야 방향벡터 정규화
	output.viewDirection = normalize(output.viewDirection);

	return output;
}