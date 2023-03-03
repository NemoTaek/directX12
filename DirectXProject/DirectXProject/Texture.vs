// �������̴�
// �̹����� ���� ��� �ؽ��ĸ� ������ ���̴�

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	// TEXCOORD �ø�ƽ�� �ȼ����̴����� �ؽ��� ��ǥ�� ��Ÿ����.
	// �ؽ��� ��ǥ�迡�� (u, v) ��ǥ�� ��Ÿ���� ���� 0~1�� ũ�⸦ ���� �ִ�.
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// ������ ��� ����� ���� ��ġ���͸� 4������ ����
	input.position.w = 1.0f;

	// world, view, projection ��Ŀ� ���� ������ ��ġ ���
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ����̴��� ����� �Է� ���� ����
	output.tex = input.tex;

	return output;
}