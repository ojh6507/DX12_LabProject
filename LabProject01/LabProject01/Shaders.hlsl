//���� ��ü�� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

//ī�޶��� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
};

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�. 
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//���� ���̴��� �����Ѵ�. 
VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    //������ ��ȯ(���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ)�Ѵ�. 
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView),gmtxProjection);
    output.color = input.color;
    output.color.rgb = pow(input.color.rgb, 1.f / 2.2f);
    return (output);
}

//�ȼ� ���̴��� �����Ѵ�. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (pow(input.color, 1.0f / 2.2f));
}