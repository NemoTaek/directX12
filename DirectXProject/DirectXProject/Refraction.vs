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
	// float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 refractionPosition : TEXCOORD1;	// ���� ���� ����

	// float3 normal : NORMAL;	// ��
	// float clip : SV_ClipDistance0;	// ��
};

PixelInputType RefractionVertexShader(VertexInputType input)
{
	matrix viewProjectWorld;
	PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
	output.tex = input.tex;

	// ������ ���� ��� ����
	viewProjectWorld = mul(viewMatrix, projectionMatrix);
	viewProjectWorld = mul(worldMatrix, viewProjectWorld);

	// ���� ���� ��ġ ���
	output.refractionPosition = mul(input.position, viewProjectWorld);

	/*
	��
	// ���� ��Ŀ� ���Ͽ� ���� ���� ���
	output.normal = mul(input.normal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);

	// Ŭ���θ��� �����Ѵ�.
	output.clip = dot(mul(input.position, worldMatrix), clipPlane);
	*/

	return output;
}