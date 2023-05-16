cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ClipPlaneBuffer
{
	float4 clipPlane;
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
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
	float clip : SV_ClipDistance0;
};

PixelInputType ReflectionVertexShader(VertexInputType input)
{
	PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ѵ�.
	output.tex = input.tex;

	// �ȼ� ���̴��� ���� ���� ���
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// ���� ���Ϳ� ���� tangent ���Ϳ� binormal ���� ���
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	// �ݻ� ��Ŀ� ���� ���� ���
	output.color = input.color;

	// Ŭ���� ��� ����
	output.clip = dot(mul(input.position, worldMatrix), clipPlane);

	return output;
}