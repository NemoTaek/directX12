// Globals

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// ���ݻ� ����� ���� ī�޶� ��ġ ����
cbuffer CameraBuffer
{
	float3 cameraPosition;
}


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
	float3 viewDirection : TEXCOORD1;	// �ݻ籤 ���꿡 �ʿ�
};


// Vertex Shader

PixelInputType BumpMapVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

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

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	// ���� ���� ��ġ ���
	worldPosition = mul(input.position, worldMatrix);

	// ī�޶� ��ġ�� ���� ���� ��ġ�� �������� �þ� ���� ���
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// �þ� ���⺤�� ����ȭ
	output.viewDirection = normalize(output.viewDirection);

	return output;
}