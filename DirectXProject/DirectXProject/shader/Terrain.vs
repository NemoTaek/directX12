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
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	// float4 color : COLOR;
	float2 tex2 : TEXCOORD1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
   	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	// float4 color : COLOR;
	// float4 depthPosition: TEXCOORD1;	// ������ �ؽ�ó�� �� �� �����ϰ� ���̵��� �ȼ��� ���� ���� �߰�
	float2 tex2 : TEXCOORD1;
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
	output.tex2 = input.tex2;

	// �ȼ� ���̴��� ���� ���� ���
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// ���� ���Ϳ� ���� tangent ���Ϳ� binormal ���� ���
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	// �ȼ� ���̴��� �÷� ���� ���� ����
	// output.color = input.color;

	// ���� �� ����� ���� ��ġ �� ����
	// output.depthPosition = output.position;

	return output;
}