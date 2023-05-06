cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// �ݻ� ȿ���� ���� �ݻ� �� ���
// �� �ݻ�� ���� �ݻ簡 �ƴ� �ſ�� ����Ͱ� ���� �ݻ� ȿ����
// �ݻ� ȭ���� �ؽ��Ŀ� �׸��� ������ ���� ȥ���Ͽ� ������
cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
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
	float4 reflectionPosition : TEXCOORD1;	// �ݻ�
	float4 refractionPosition : TEXCOORD2;	// ����
};

PixelInputType WaterVertexShader(VertexInputType input)
{
	PixelInputType output;
	matrix reflectProjectWorld;
	matrix viewProjectWorld;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
	output.tex = input.tex;

	// �ݻ� �������� ��� ����
	reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// �ݻ� ��Ŀ� ���� �Է� ��ġ ���
	output.reflectionPosition = mul(input.position, reflectProjectWorld);

	// ���� �������� ��� ����
	viewProjectWorld = mul(viewMatrix, projectionMatrix);
	viewProjectWorld = mul(worldMatrix, viewProjectWorld);

	// ���� ��Ŀ� ���� �Է� ��ġ ���
	output.refractionPosition = mul(input.position, viewProjectWorld);

	return output;
}