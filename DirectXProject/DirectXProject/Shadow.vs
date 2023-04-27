cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	// ������ ������ ������ 2���� ���
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightBuffer2
{
	float3 lightPosition;
	float padding;
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

	// ���� �������� ��ȯ�� ������ ��ġ
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

PixelInputType ShadowVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// ������ ��� ����� ���� ��ġ���͸� 4������ ����
	input.position.w = 1.0f;

	// world, view, projection ��Ŀ� ���� ������ ��ġ ���
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// ������ ���� �������� ������ ��ġ ���
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// �ȼ����̴��� ����� �ؽ��� ����
	output.tex = input.tex;

	// ���� ��Ŀ� ���� �������� ���
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// �������� ����ȭ
	output.normal = normalize(output.normal);

	// ���� ���� ��ġ ���
	worldPosition = mul(input.position, worldMatrix);

	// ������ ��ġ�� ���� ���� ��ġ�� ������� ���� ��ġ ����
	output.lightPos = lightPosition.xyz - worldPosition.xyz;

	// �� ��ġ���� ����ȭ
	output.lightPos = normalize(output.lightPos);

	return output;
}