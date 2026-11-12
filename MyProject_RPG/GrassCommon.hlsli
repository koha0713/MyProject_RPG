#ifndef GRASS_COMMON_HLSLI
#define GRASS_COMMON_HLSLI

cbuffer GrassConstantBuffer :
	register(b0)
{
    float4x4 ViewProjection;

	// x = Time
	// y = WindStrength
	// z = WindSpeed
	// w = AlphaClip
    float4 GrassParameters;

	// xy = WindDirection
    float4 WindParameters;
};

#endif