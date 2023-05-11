cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 클리핑 평면은 기하학적 구조를 잘라내는데 사용된다.
// 예시로 near 평면과 far 평면이 있는데, far 평면 뒤로 이어지는 것들을 자를 때 사용한다.
// 부동소수점 벡터 4개를 가지는데, x y z축에서 w축을 기준으로 그 밖의 영역은 그려지지 않도록 한다.
cbuffer ClipPlaneBuffer
{
	float4 clipPlane;
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
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표 및 색상을 저장한다.
	output.tex = input.tex;

	return output;
}