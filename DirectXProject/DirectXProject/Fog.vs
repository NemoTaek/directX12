cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer FogBuffer
{
	float fogStart;
	float fogEnd;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;	// 정점 셰이더에서 계산하고 픽셀 셰이더에 값을 넘겨준다.
};

PixelInputType FogVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 cameraPosition;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 카메라 위치를 계산한다.
	cameraPosition = mul(input.position, worldMatrix);
	cameraPosition = mul(cameraPosition, viewMatrix);

	// Linear Fog 계산한다.
	// 선형 안개 공식은 안개 안에서의 거리에 따라 농도를 선형적으로 다르게 하는 식입니다. (여기서 거리는 cameraPosition)
	// Linear Fog = (fogEnd - ViewpointDistance) / (fogEnd - fogStart)
	// 추가로 Exponential Fog 공식도 있다.
	// Exponential Fog1 = 1 / e^(ViewpointDistance * FogDensity)
	// Exponential Fog2 = 1 / e^((ViewpointDistance * FogDensity) * (ViewpointDistance * FogDensity))
	output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
	// output.fogFactor = saturate(1 / exp(cameraPosition.z * 0.5));

	return output;
}