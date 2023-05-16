cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ClipPlaneBuffer
{
	float4 clipPlane;
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
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
	float clip : SV_ClipDistance0;
};

PixelInputType ReflectionVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 픽셀 쉐이더의 법선 벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// 법선 벡터에 대한 tangent 벡터와 binormal 벡터 계산
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	// 반사 행렬에 대한 색상 계산
	output.color = input.color;

	// 클리핑 평면 설정
	output.clip = dot(mul(input.position, worldMatrix), clipPlane);

	return output;
}