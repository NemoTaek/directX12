cbuffer TessellationBuffer
{
	float tessellationAmount;
	float3 padding;
};

struct HullInputType
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct ConstantOutputType
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
	float3 position : POSITION;
	float4 color : COLOR;
};

// ��ġ ��� �Լ�
// ��ü ��ġ���� ���� ������ �ʴ� �����͸� ����, ����ϴµ� ���
// ���⼭�� �ﰢ���� �� �������� �߽����� ���� �׼����̼� ����� ����
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	// �ﰢ���� �� �𼭸��� ���� �׼����̼� ��� ����
	output.edges[0] = tessellationAmount;
	output.edges[1] = tessellationAmount;
	output.edges[2] = tessellationAmount;

	// �ﰢ�� �߽����� ���� �׼����̼� ��� ����
	output.inside = tessellationAmount;

	return output;
}

// �� ���̴�
// ��µǴ� �� ���������� ��ġ ����Լ��� �� ���̴��� ȣ��
// ���⼭�� ���� ����� �������� ������ ���̴��� ����
[domain("tri")]										// ��ġ�� ����: �ﰢ��
[partitioning("integer")]							// �׼����̼� ���� ����: ����
[outputtopology("triangle_cw")]						// �׼������Ͱ� ����� �⺻ ������ ����: �ﰢ��
[outputcontrolpoints(3)]							// ����� �������� ��: 3��
[patchconstantfunc("ColorPatchConstantFunction")]	// �׼������Ϳ� ������ �� ��ġ ��� �����͸� �����ϱ� ���� ȣ���� ��� �Լ��� �̸�
HullOutputType TessellationHullShader(InputPatch<HullInputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOutputType output;

	// ���� �������� ��ġ�� ��� ��ġ�� ���
	output.position = patch[pointId].position;

	// �Է� ������ ��� �������� ���
	output.color = patch[pointId].color;

	return output;
}