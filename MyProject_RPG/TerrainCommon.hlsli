#ifndef TERRAIN_COMMON_HLSLI
#define TERRAIN_COMMON_HLSLI

cbuffer TerrainConstantBuffer :
	register(b0)
{
    float4x4 World;

    float4x4 WorldViewProjection;

	// x = Tiling
    float4 TerrainParameters;

    float4 TerrainColor;

	// xyz = Direction
	// w = Intensity
    float4 LightDirectionIntensity;

    float4 LightColor;

    float4 AmbientLightColor;

	// x = HighlightThreshold
	// y = ShadowThreshold
	// z = MidToneIntensity
	// w = ShadowIntensity
    float4 ToonParameters;
};

#endif