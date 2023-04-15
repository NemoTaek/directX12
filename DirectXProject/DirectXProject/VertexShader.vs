// 콜론 뒤에 register(b0)는 논리 레지스터 공간 내의 가상 레지스터에 바인딩 한다는 의미이다.
// t: SRV(Shader Resource View), s: sampler, u: UAV(Unordered Access View), b: CBV(Constant Buffer View)
// 여기서는 버퍼 0번슬롯에 할당한다는 뜻이다.
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 정반사 계산을 위한 카메라 위치 정보
cbuffer CameraBuffer : register(b1)
{
	float3 cameraPosition;
	float padding;
}

struct VertexInputType
{
	float4 position : POSITION;

	// float4 color : COLOR;

	// TEXCOORD 시맨틱은 픽셀셰이더에서 텍스쳐 좌표를 나타낸다.
	// 텍스쳐 좌표계에는 (u, v) 좌표로 나타내며 각각 0~1의 크기를 갖고 있다.
	float2 tex : TEXCOORD0;
	
	// 방향 조명
	// 방향벡터와 법선벡터가 필요
	// 방향벡터는 직접 지정, 법선벡터는 정점들에 의해 만들어지는 평면으로 계산
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	// float4 color : COLOR;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	// float3 viewDirection : TEXCOORD1;	// 시야 방향
};

PixelInputType VS(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// 적절한 행렬 계산을 위해 위치벡터를 4단위로 변경
	input.position.w = 1.0f;

	// world, view, projection 행렬에 대한 정점의 위치 계산
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀셰이더가 사용할 입력 색상 저장
	// output.color = input.color;

	// 픽셀셰이더가 사용할 텍스쳐 저장
	output.tex = input.tex;

	// 세계 행렬에 대한 법선벡터 계산
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 법선벡터 정규화
	output.normal = normalize(output.normal);

	// 세계 정점 위치 계산
	// worldPosition = mul(input.position, worldMatrix);

	// 카메라 위치와 세계 정점 위치를 기준으로 시야 방향 계산
	// output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// 시야 방향벡터 정규화
	// output.viewDirection = normalize(output.viewDirection);

	return output;
}