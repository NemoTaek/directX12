cbuffer MatrixBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Post-multiplied: 벡터가 행렬 뒤에 놓이고, 행렬의 결합 방향은 우에서 좌로 흐른다. (Translation Matrix * Rotation Matrix * Scale Matrix * V)
// Pre-multiplied: 벡터가 행렬 앞에 놓이고, 행렬의 결합 방향은 좌에서 우로 흐른다. (V * Scale Matrix * Rotation Matrix * Translation Matrix)
// column-major: 행렬을 배열로 나타낼 때, column 방향(위에서 아래 방향)으로 먼저 배치한다.
// row-major: 행렬을 배열로 나타낼 때, row 방향(좌에서 우 방향)으로 먼저 배치한다.
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

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, input.instanceWorld);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표 및 색상을 저장한다.
	output.tex = input.tex;

	output.foliageColor = input.instanceColor;

	return output;
}