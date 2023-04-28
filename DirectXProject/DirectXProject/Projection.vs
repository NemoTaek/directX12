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

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// ������ ���� �������� ������ ��ġ ���
	output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.viewPosition, viewMatrix2);
	output.viewPosition = mul(output.viewPosition, projectionMatrix2);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� ����
	output.tex = input.tex;

	// ���� ��Ŀ� ���� ���� ���� ���
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}