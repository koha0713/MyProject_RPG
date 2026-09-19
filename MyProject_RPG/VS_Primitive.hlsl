cbuffer PrimitiveConstantBuffer : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
};

struct VSInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VSOutput main(
	VSInput input)
{
    VSOutput output;

    float4 position =
		float4(
			input.Position,
			1.0f);

    position =
		mul(
			position,
			View);

    position =
		mul(
			position,
			Projection);

    output.Position =
		position;

    output.Color =
		input.Color;

    return output;
}