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

	float TerrainIndex : TEXCOORD2;
};

/**
 * @brief TerrainŽí—Þ‚É‰ž‚¶‚ÄTexture‚ð‘I‘ð
 */
float4 SampleTerrainTexture(
	float terrainIndex,
	float2 uv)
{
	//=============================================
	// Grass
	//=============================================

    if (terrainIndex < 0.5f)
    {
        return
			GrassTexture.Sample(
				TerrainSampler,
				uv);
    }

	//=============================================
	// Dirt
	//=============================================

    if (terrainIndex < 1.5f)
    {
        return
			DirtTexture.Sample(
				TerrainSampler,
				uv);
    }

	//=============================================
	// Rock
	//=============================================

    if (terrainIndex < 2.5f)
    {
        return
			RockTexture.Sample(
				TerrainSampler,
				uv);
    }

	//=============================================
	// Sand
	//=============================================

    return
		SandTexture.Sample(
			TerrainSampler,
			uv);
}

float4 main(
	PSInput input) : SV_TARGET
{
	//=================================================
	// World Space UV
	//=================================================

    const float2 terrainUV =
		input.WorldPosition.xz *
		TerrainParameters.x;

	//=================================================
	// Terrain Texture
	//=================================================

    const float4 textureColor =
		SampleTerrainTexture(
			input.TerrainIndex,
			terrainUV);

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
		textureColor.rgb *
		TerrainColor.rgb *
		lighting;

    return float4(
		saturate(
			finalColor),
		1.0f);
}