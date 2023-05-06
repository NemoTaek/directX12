cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 불 효과는 설정할 것이 많아서 따로 뺐습니다.
// 유리, 얼음 효과처럼 텍스처의 샘플링 위치를 흩뜨리는 것은 같지만, 노이즈 텍스처를 수정하고 텍스처 좌표를 흩뜨리는 방법이 다릅니다.
// 불 효과에는 노이즈 텍스처가 3가지 필요합니다. (불 형태를 보이는 텍스처, 불의 색상과 불의 경계가 보이는 노이즈 텍스처, 투명도를 위한 알파 텍스처)
// 이를 잘 설정하여 새로운 텍스처를 만들고 이를 스크롤하여 불 효과를 내게 합니다.
// 그 후 더 불 효과를 잘 내기 위해 노이즈 텍스처의 범위를 수정합니다. 
// 다음은 원본 불꽃 색상 텍스처를 왜곡 스케일과 여백 값을 사용하여 흩뜨려 불꽃이 흔들리는 효과를 냅니다.
cbuffer NoiseBuffer
{
	float frameTime;		// 프레임 시간마다 노이즈 텍스처들을 스크롤하는 변수
	float3 scrollSpeeds;	// 서로 다른 스크롤 속도 값을 저장하는 변수
	float3 scales;			// 노이즈 텍스처의 서로 다른 스케일 값을 저장하는 변수
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

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는 것으로 사용한다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 사영의 순으로 계산.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 각 노이즈 텍스처에 대한 텍스처 좌표 계산
	output.texCoords1 = (input.tex * scales.x);
	output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);
	output.texCoords2 = (input.tex * scales.y);
	output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);
	output.texCoords3 = (input.tex * scales.z);
	output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);

	return output;
}