cbuffer cbPlayerInfo : register(b0)
{
    matrix gmtxPlayerWorld : packoffset(c0);
};
//ī�޶� ��ü�� �����͸� ���� ��� ����(����ŧ�� ���� ����� ���Ͽ� ī�޶��� ��ġ ���͸� �߰�)
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};
//���� ��ü�� �����͸� ���� ��� ����(���� ��ü�� ���� ���� ��ȣ�� �߰�)
cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
    uint gnMaterial : packoffset(c4);
};
cbuffer BulletBuffer : register(b5)
{
    matrix worldMatrix : packoffset(c0);
};
#include "Light.hlsl"

struct VS_DIFFUSED_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};
//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_DIFFUSED_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}
float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (pow(input.color, 1.f / 2.2f));
}


VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), worldMatrix), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}
float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (pow(input.color, 1.f / 2.2f));
}


//���� ������ ���
#define _WITH_VERTEX_LIGHTING
//���� ���̴��� �Է� ���� ����
struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};
struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#else
    float3 normalW : NORMAL;
#endif
};

//���� ���̴� �Լ�
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.color = Lighting(output.positionW, normalize(normalW));
#else
output.normalW = normalW;
#endif
    return (output);
}

//�ȼ� ���̴� �Լ�
float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (pow(input.color, 1.f/2.2f));
#else
float3 normalW = normalize(input.normalW);
float4 color = Lighting(input.positionW, normalW);
return(color);
#endif
}