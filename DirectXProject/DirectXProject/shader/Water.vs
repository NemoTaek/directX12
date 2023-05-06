cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 반사 효과를 위한 반사 뷰 행렬
// 이 반사는 조명 반사가 아닌 거울로 비춘것과 같은 반사 효과로
// 반사 화면을 텍스쳐에 그리고 보여질 곳과 혼합하여 렌더링
cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
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
	float4 reflectionPosition : TEXCOORD1;	// 반사
	float4 refractionPosition : TEXCOORD2;	// 굴절
};

PixelInputType WaterVertexShader(VertexInputType input)
{
	PixelInputType output;
	matrix reflectProjectWorld;
	matrix viewProjectWorld;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 반사 투영세계 행렬 생성
	reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// 반사 행렬에 대한 입력 위치 계산
	output.reflectionPosition = mul(input.position, reflectProjectWorld);

	// 굴절 투영세계 행렬 생성
	viewProjectWorld = mul(viewMatrix, projectionMatrix);
	viewProjectWorld = mul(worldMatrix, viewProjectWorld);

	// 굴절 행렬에 대한 입력 위치 계산
	output.refractionPosition = mul(input.position, viewProjectWorld);

	return output;
}