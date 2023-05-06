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
	float4 color : COLOR;
	// float3 instancePosition : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
	// float clip : SV_ClipDistance0;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// Ư�� �ν��Ͻ��� ���� �����͸� ������� ������ ��ġ�� ����
	// input.position.x += input.instancePosition.x;
	// input.position.y += input.instancePosition.y;
	// input.position.z += input.instancePosition.z;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ �� ������ �����Ѵ�.
	output.tex = input.tex;
	output.color = input.color;

	// Ŭ���θ��� �����Ѵ�.
	// output.clip = dot(mul(input.position, worldMatrix), clipPlane);

	return output;
}