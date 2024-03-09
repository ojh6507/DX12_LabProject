float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
{
    float4 output;
    if (nVertexID == 0)
        output = float4(0.0f, 0.5f, 0.5f, 1.0f);
    else if (nVertexID == 1)
        output = float4(0.5f, -0.5f, 0.5f, 1.0f);
    else if (nVertexID == 2)
        output = float4(-0.5f, -0.5f, 0.5f, 1.0f);
    return output;
}

float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    return (float4(1.0f, 1.0f, 0.0f, 1.0f));
}