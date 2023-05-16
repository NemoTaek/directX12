cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix reflectionMatrix;
};

cbuffer CamNormBuffer
{
	float3 cameraPosition;
	float padding1;
	float2 normalMapTiling;
	float2 padding2;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 reflectionPosition : TEXCOORD0;	// �ݻ�
	float4 refractionPosition : TEXCOORD1;	// ����
	float3 viewDirection : TEXCOORD2;
	float2 tex1 : TEXCOORD3;
	float2 tex2 : TEXCOORD4;
};

PixelInputType WaterVertexShader(VertexInputType input)
{
	PixelInputType output;
	matrix reflectProjectWorld;
	matrix viewProjectWorld;
	float4 worldPosition;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

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

	// ���� ���� ��ġ ���
	worldPosition = mul(input.position, worldMatrix);

	// ī�޶� ��ġ�� ���� ���� ��ġ�� �������� �þ� ���� ����
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	// ���濡 ���� ���� Ÿ�ϸ�
	output.tex1 = input.tex / normalMapTiling.x;
	output.tex2 = input.tex / normalMapTiling.y;

	return output;
}