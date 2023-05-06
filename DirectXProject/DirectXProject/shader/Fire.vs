cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// �� ȿ���� ������ ���� ���Ƽ� ���� �����ϴ�.
// ����, ���� ȿ��ó�� �ؽ�ó�� ���ø� ��ġ�� ��߸��� ���� ������, ������ �ؽ�ó�� �����ϰ� �ؽ�ó ��ǥ�� ��߸��� ����� �ٸ��ϴ�.
// �� ȿ������ ������ �ؽ�ó�� 3���� �ʿ��մϴ�. (�� ���¸� ���̴� �ؽ�ó, ���� ����� ���� ��谡 ���̴� ������ �ؽ�ó, ������ ���� ���� �ؽ�ó)
// �̸� �� �����Ͽ� ���ο� �ؽ�ó�� ����� �̸� ��ũ���Ͽ� �� ȿ���� ���� �մϴ�.
// �� �� �� �� ȿ���� �� ���� ���� ������ �ؽ�ó�� ������ �����մϴ�. 
// ������ ���� �Ҳ� ���� �ؽ�ó�� �ְ� �����ϰ� ���� ���� ����Ͽ� ��߷� �Ҳ��� ��鸮�� ȿ���� ���ϴ�.
cbuffer NoiseBuffer
{
	float frameTime;		// ������ �ð����� ������ �ؽ�ó���� ��ũ���ϴ� ����
	float3 scrollSpeeds;	// ���� �ٸ� ��ũ�� �ӵ� ���� �����ϴ� ����
	float3 scales;			// ������ �ؽ�ó�� ���� �ٸ� ������ ���� �����ϴ� ����
	float padding;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoords1 : TEXCOORD1;
	float2 texCoords2 : TEXCOORD2;
	float2 texCoords3 : TEXCOORD3;
};

PixelInputType FireVertexShader(VertexInputType input)
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

	// �� ������ �ؽ�ó�� ���� �ؽ�ó ��ǥ ���
	output.texCoords1 = (input.tex * scales.x);
	output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);
	output.texCoords2 = (input.tex * scales.y);
	output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);
	output.texCoords3 = (input.tex * scales.z);
	output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);

	return output;
}