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
	float3 position : POSITION;
	float4 color : COLOR;

	// TEXCOORD 시맨틱은 픽셀셰이더에서 텍스쳐 좌표를 나타낸다.
	// 텍스쳐 좌표계에는 (u, v) 좌표로 나타내며 각각 0~1의 크기를 갖고 있다.
	// float2 tex : TEXCOORD0;
	
	// 방향 조명
	// 방향벡터와 법선벡터가 필요
	// 방향벡터는 직접 지정, 법선벡터는 정점들에 의해 만들어지는 평면으로 계산
	// float3 normal : NORMAL;
};

// 소프트웨어에서 처리하면 그래픽카드에 수많은 다각형을 보내어 비효율적이고 렌더링 성능에도 병목 현상이 일어날 수 있음
// 테셀레이션을 하드웨어에서 처리하기 위하여 Hull Shader와 Domian Shader 과정을 추가
// 기본 다각형을 그래픽카드에 보내고 그래픽카드 안에서 분할 알고리즘을 수행
struct HullInputType
{
	float3 position : POSITION;
	float4 color : COLOR;
};

HullInputType TessellationVertexShader(VertexInputType input)
{
	HullInputType output;

	// 정점의 위치 계산
	output.position = input.position;

	// 픽셀셰이더가 사용할 입력 색상 저장
	output.color = input.color;

	return output;
}