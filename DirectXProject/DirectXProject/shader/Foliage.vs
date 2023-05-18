cbuffer MatrixBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Post-multiplied: ���Ͱ� ��� �ڿ� ���̰�, ����� ���� ������ �쿡�� �·� �帥��. (Translation Matrix * Rotation Matrix * Scale Matrix * V)
// Pre-multiplied: ���Ͱ� ��� �տ� ���̰�, ����� ���� ������ �¿��� ��� �帥��. (V * Scale Matrix * Rotation Matrix * Translation Matrix)
// column-major: ����� �迭�� ��Ÿ�� ��, column ����(������ �Ʒ� ����)���� ���� ��ġ�Ѵ�.
// row-major: ����� �迭�� ��Ÿ�� ��, row ����(�¿��� �� ����)���� ���� ��ġ�Ѵ�.
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	row_major matrix instanceWorld : WORLD;
	float3 instanceColor : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 foliageColor : TEXCOORD1;
};

PixelInputType FoliageVertexShader(VertexInputType input)
{
	PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ� ������ ����Ѵ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, �翵�� ������ ���.
	output.position = mul(input.position, input.instanceWorld);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ�ó ��ǥ �� ������ �����Ѵ�.
	output.tex = input.tex;

	output.foliageColor = input.instanceColor;

	return output;
}