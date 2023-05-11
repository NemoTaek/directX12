cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Ŭ���� ����� �������� ������ �߶󳻴µ� ���ȴ�.
// ���÷� near ���� far ����� �ִµ�, far ��� �ڷ� �̾����� �͵��� �ڸ� �� ����Ѵ�.
// �ε��Ҽ��� ���� 4���� �����µ�, x y z�࿡�� w���� �������� �� ���� ������ �׷����� �ʵ��� �Ѵ�.
cbuffer ClipPlaneBuffer
{
	float4 clipPlane;
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
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ �� ������ �����Ѵ�.
	output.tex = input.tex;

	return output;
}