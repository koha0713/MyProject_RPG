#include "TerrainCommon.hlsli"

struct VSInput
{
    float3 Position : POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD;

	// Terrainî‘çÜÇÕfloatÇ∆ÇµÇƒéÛÇØéÊÇÈ
    float TerrainIndex : TERRAININDEX;
};

struct VSOutput
{
    float4 Position : SV_POSITION;

    float3 WorldPosition : TEXCOORD0;

    float3 WorldNormal : TEXCOORD1;

    float TerrainIndex : TEXCOORD2;
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

    output.WorldNormal =
		normalize(
			mul(
				float4(
					input.Normal,
					0.0f),
				World).xyz);

    output.TerrainIndex =
		input.TerrainIndex;

    return output;
}