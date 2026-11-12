#include "WaterCommon.hlsli"

struct VSInput
{
    float3 Position : POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD;

    /**
     * êÖñ Ç©ÇÁêÏíÍÇ‹Ç≈ÇÃãóó£
     */
    float WaterDepth : WATERDEPTH;
};

struct VSOutput
{
    float4 Position : SV_POSITION;

    float3 WorldPosition : TEXCOORD0;

    /**
     * Pixel ShaderÇ÷ï‚ä‘ÇµÇƒìnÇ∑ÅB
     */
    float WaterDepth : TEXCOORD1;
};

VSOutput main(
    VSInput input)
{
    VSOutput output;

    const float4 localPosition =
        float4(
            input.Position,
            1.0f);

    output.Position =
        mul(
            localPosition,
            WorldViewProjection);

    output.WorldPosition =
        mul(
            localPosition,
            World).xyz;

    output.WaterDepth =
        input.WaterDepth;

    return output;
}