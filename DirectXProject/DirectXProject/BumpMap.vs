// Globals

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


// Typedefs

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;		// z��(ź��Ʈ ���������� ������ �Ǵ� ����)
	float3 tangent : TANGENT;	// x��(uv ��ǥ�迡���� u��ǥ)
	float3 binormal : BINORMAL;	// y��(uv ��ǥ�迡���� v��ǥ)
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;		// z��(ź��Ʈ ���������� ������ �Ǵ� ����)
	float3 tangent : TANGENT;	// x��(uv ��ǥ�迡���� u��ǥ)
	float3 binormal : BINORMAL;	// y��(uv ��ǥ�迡���� v��ǥ)
};


// Vertex Shader

PixelInputType BumpMapVertexShader(VertexInputType input)
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

	// ���� ��Ŀ� ���ؼ��� �� ���͸� ����� ���� ���� ���� ����ȭ�Ѵ�.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.normal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	return output;
}