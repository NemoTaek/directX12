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
	float fogFactor : FOG;	// ���� ���̴����� ����ϰ� �ȼ� ���̴��� ���� �Ѱ��ش�.
};

PixelInputType FogVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 cameraPosition;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
	output.tex = input.tex;

	// ī�޶� ��ġ�� ����Ѵ�.
	cameraPosition = mul(input.position, worldMatrix);
	cameraPosition = mul(cameraPosition, viewMatrix);

	// Linear Fog ����Ѵ�.
	// ���� �Ȱ� ������ �Ȱ� �ȿ����� �Ÿ��� ���� �󵵸� ���������� �ٸ��� �ϴ� ���Դϴ�. (���⼭ �Ÿ��� cameraPosition)
	// Linear Fog = (fogEnd - ViewpointDistance) / (fogEnd - fogStart)
	// �߰��� Exponential Fog ���ĵ� �ִ�.
	// Exponential Fog1 = 1 / e^(ViewpointDistance * FogDensity)
	// Exponential Fog2 = 1 / e^((ViewpointDistance * FogDensity) * (ViewpointDistance * FogDensity))
	output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
	// output.fogFactor = saturate(1 / exp(cameraPosition.z * 0.5));

	return output;
}