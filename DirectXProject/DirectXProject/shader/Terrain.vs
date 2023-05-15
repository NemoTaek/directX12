cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
	float4 tex : TEXCOORD0;
    float3 normal : NORMAL;
	// float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 tex: TEXCOORD0;
    float3 normal : NORMAL;
	// float4 color : COLOR;
	// float4 depthPosition: TEXCOORD1;	// ������ �ؽ�ó�� �� �� �����ϰ� ���̵��� �ȼ��� ���� ���� �߰�
};

PixelInputType TerrainVertexShader(VertexInputType input)
{
    PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ ����
	output.tex = input.tex;

	// �ȼ� ���̴��� ���� ���� ���
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// �ȼ� ���̴��� �÷� ���� ���� ����
	// output.color = input.color;

	// ���� �� ����� ���� ��ġ �� ����
	// output.depthPosition = output.position;

	return output;
}