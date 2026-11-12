#include "TerrainCommon.hlsli"

Texture2D GrassTexture :
	register(t0);

Texture2D DirtTexture :
	register(t1);

Texture2D RockTexture :
	register(t2);

Texture2D SandTexture :
	register(t3);

SamplerState TerrainSampler :
	register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;

    float3 WorldPosition : TEXCOORD0;

    float3 WorldNormal : TEXCOORD1;

    float2 LocalUV : TEXCOORD2;

	// x = Center
	// y = Up
	// z = Right
	// w = Down
    float4 TerrainInfo : TEXCOORD3;

	// x = Left
    float4 TerrainInfo2 : TEXCOORD4;
};


//=====================================================
// Sample Terrain Texture
//=====================================================

float4 SampleTerrainTexture(
	float terrainIndex,
	float2 uv)
{
    if (terrainIndex < 0.5f)
    {
        return
			GrassTexture.Sample(
				TerrainSampler,
				uv);
    }

    if (terrainIndex < 1.5f)
    {
        return
			DirtTexture.Sample(
				TerrainSampler,
				uv);
    }

    if (terrainIndex < 2.5f)
    {
        return
			RockTexture.Sample(
				TerrainSampler,
				uv);
    }

    return
		SandTexture.Sample(
			TerrainSampler,
			uv);
}


//=====================================================
// Main
//=====================================================

float4 main(
	PSInput input) : SV_TARGET
{
	//=================================================
	// Terrain Information
	//=================================================

    const float centerTerrain =
		input.TerrainInfo.x;

    const float terrainUp =
		input.TerrainInfo.y;

    const float terrainRight =
		input.TerrainInfo.z;

    const float terrainDown =
		input.TerrainInfo.w;

    const float terrainLeft =
		input.TerrainInfo2.x;

	//=================================================
	// World Space UV
	//=================================================

    const float2 terrainUV =
		input.WorldPosition.xz *
		TerrainParameters.x;

	//=================================================
	// Base Terrain
	//=================================================

    float4 terrainColor =
		SampleTerrainTexture(
			centerTerrain,
			terrainUV);

	//=================================================
	// Border Blend
	//=================================================
	//
	// 1Cell全体をBlendするのではなく、
	// Cell端から18%程度だけを使用する。
	//
	// さらに共有境界では最大0.5までしか混ぜない。
	//
	// これにより、
	//
	// Grass側 = Grass50% + Dirt50%
	// Dirt側  = Dirt50%  + Grass50%
	//
	// となり境界の色が一致する。
	//=================================================

    const float BLEND_WIDTH =
		0.18f;

	//=================================================
	// Up
	//=================================================

    if (abs(
		terrainUp -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				BLEND_WIDTH,
				0.0f,
				input.LocalUV.y) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainUp,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Right
	//=================================================

    if (abs(
		terrainRight -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				1.0f -
					BLEND_WIDTH,
				1.0f,
				input.LocalUV.x) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainRight,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Down
	//=================================================

    if (abs(
		terrainDown -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				1.0f -
					BLEND_WIDTH,
				1.0f,
				input.LocalUV.y) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainDown,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Left
	//=================================================

    if (abs(
		terrainLeft -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				BLEND_WIDTH,
				0.0f,
				input.LocalUV.x) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainLeft,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Lighting
	//=================================================

    const float3 normal =
		normalize(
			input.WorldNormal);

    const float3 lightDirection =
		normalize(
			-LightDirectionIntensity.xyz);

    const float ndotl =
		saturate(
			dot(
				normal,
				lightDirection));

	//=================================================
	// Toon
	//=================================================

    float toonIntensity =
		1.0f;

    if (ndotl >=
		ToonParameters.x)
    {
        toonIntensity =
			1.0f;
    }
    else if (ndotl >=
		ToonParameters.y)
    {
        toonIntensity =
			ToonParameters.z;
    }
    else
    {
        toonIntensity =
			ToonParameters.w;
    }

	//=================================================
	// Lighting Color
	//=================================================

    const float3 directional =
		LightColor.rgb *
		LightDirectionIntensity.w *
		toonIntensity;

    const float3 lighting =
		AmbientLightColor.rgb +
		directional;

	//=================================================
	// Final
	//=================================================

    const float3 finalColor =
		terrainColor.rgb *
		TerrainColor.rgb *
		lighting;

    return float4(
		saturate(
			finalColor),
		1.0f);
}