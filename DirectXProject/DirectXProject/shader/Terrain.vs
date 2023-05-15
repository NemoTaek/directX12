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
	// float4 depthPosition: TEXCOORD1;	// 근접한 텍스처를 볼 때 선명하게 보이도록 픽셀의 깊이 변수 추가
};

PixelInputType TerrainVertexShader(VertexInputType input)
{
    PixelInputType output;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표 저장
	output.tex = input.tex;

	// 픽셀 쉐이더의 법선 벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// 픽셀 쉐이더에 컬러 맵의 색상 전달
	// output.color = input.color;

	// 깊이 값 계산을 위해 위치 값 저장
	// output.depthPosition = output.position;

	return output;
}