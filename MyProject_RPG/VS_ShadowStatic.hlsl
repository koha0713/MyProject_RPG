//=====================================================
// Static Shadow Vertex Shader
//=====================================================

cbuffer ShadowCastConstantBuffer :
	register(b0)
{
    float4x4 LightWorldViewProjection;
};

struct VSInput
{
    float3 Position :
		POSITION;
};

float4 main(
	VSInput input) : SV_POSITION
{
    return mul(
		float4(
			input.Position,
			1.0f),
		LightWorldViewProjection);
}