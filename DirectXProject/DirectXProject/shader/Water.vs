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
	float4 reflectionPosition : TEXCOORD0;	// 반사
	float4 refractionPosition : TEXCOORD1;	// 굴절
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

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

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

	// 세계 정점 위치 계산
	worldPosition = mul(input.position, worldMatrix);

	// 카메라 위치와 세계 정점 위치를 기준으로 시야 방향 결정
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	// 물길에 법선 맵을 타일링
	output.tex1 = input.tex / normalMapTiling.x;
	output.tex2 = input.tex / normalMapTiling.y;

	return output;
}