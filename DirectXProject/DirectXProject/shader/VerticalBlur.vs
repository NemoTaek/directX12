cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ScreenSizeBuffer
{
	float screenHeight;
	float3 padding;
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

	// 가운데 픽셀과 양쪽으로 4개씩 인접한 픽셀의 UV 위치를 가짐
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
	float2 texCoord9 : TEXCOORD9;
};

PixelInputType VerticalBlurVertexShader(VertexInputType input)
{
	PixelInputType output;
	float texelSize;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 화면 너비에 대한 텍셀의 크기를 구함. 이 값으로 인접한 픽셀의 UV 좌표를 계산
	texelSize = 1.0f / screenHeight;

	output.texCoord1 = input.tex + float2(0.0f, texelSize * -4.0f);
	output.texCoord2 = input.tex + float2(0.0f, texelSize * -3.0f);
	output.texCoord3 = input.tex + float2(0.0f, texelSize * -2.0f);
	output.texCoord4 = input.tex + float2(0.0f, texelSize * -1.0f);
	output.texCoord5 = input.tex + float2(0.0f, texelSize * 0.0f);
	output.texCoord6 = input.tex + float2(0.0f, texelSize * 1.0f);
	output.texCoord7 = input.tex + float2(0.0f, texelSize * 2.0f);
	output.texCoord8 = input.tex + float2(0.0f, texelSize * 3.0f);
	output.texCoord9 = input.tex + float2(0.0f, texelSize * 4.0f);

	return output;
}