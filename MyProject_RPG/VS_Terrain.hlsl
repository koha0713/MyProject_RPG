#include "TerrainCommon.hlsli"

struct VSInput
{
    float3 Position : POSITION;

    float3 Normal : NORMAL;

    float2 TexCoord : TEXCOORD;

    float TerrainIndex : TERRAININDEX;

	// x = Up
	// y = Right
	// z = Down
	// w = Left
    float4 NeighborTerrain : NEIGHBORTERRAIN;
};

struct VSOutput
{
    float4 Position : SV_POSITION;

    float3 WorldPosition : TEXCOORD0;

    float3 WorldNormal : TEXCOORD1;

	// Cellì‡0Å`1
    float2 LocalUV : TEXCOORD2;

	// x = Center Terrain
	// y = Up
	// z = Right
	// w = Down
    float4 TerrainInfo : TEXCOORD3;

	// x = Left
    float4 TerrainInfo2 : TEXCOORD4;
};

VSOutput main(
	VSInput input)
{
    VSOutput output;

    const float4 localPosition =
		float4(
			input.Position,
			1.0f);

	//=================================================
	// Position
	//=================================================

    output.Position =
		mul(
			localPosition,
			WorldViewProjection);

    output.WorldPosition =
		mul(
			localPosition,
			World).xyz;

	//=================================================
	// Normal
	//=================================================

    output.WorldNormal =
		normalize(
			mul(
				float4(
					input.Normal,
					0.0f),
				World).xyz);

	//=================================================
	// Terrain Data
	//=================================================

    output.LocalUV =
		input.TexCoord;

    output.TerrainInfo =
		float4(
			input.TerrainIndex,
			input.NeighborTerrain.x,
			input.NeighborTerrain.y,
			input.NeighborTerrain.z);

    output.TerrainInfo2 =
		float4(
			input.NeighborTerrain.w,
			0.0f,
			0.0f,
			0.0f);

    return output;
}