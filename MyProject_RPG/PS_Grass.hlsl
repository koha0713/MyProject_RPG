#include "GrassCommon.hlsli"
#include "ShadowCommon.hlsli"

Texture2D GrassTexture :
	register(t0);

Texture2D VisibilityTexture :
	register(t4);

SamplerState GrassSampler :
	register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;

    float2 TexCoord : TEXCOORD0;

    float Random : TEXCOORD1;
	
    float3 WorldPosition : TEXCOORD2;
};

float GetGrassVisibility(
	float3 worldPosition)
{
    const float cellSize =
		max(
			VisibilityParameters.x,
			0.0001f);

    const int gridX =
		(int) floor(
			worldPosition.x /
				cellSize +
			0.5f);

    const int gridY =
		(int) floor(
			worldPosition.z /
				cellSize +
			0.5f);

    uint width;
    uint height;

    VisibilityTexture.
		GetDimensions(
			width,
			height);

    if (gridX < 0 ||
		gridY < 0 ||
		gridX >= (int) width ||
		gridY >= (int) height)
    {
        return 0.0f;
    }

    return VisibilityTexture.
		Load(
			int3(
				gridX,
				gridY,
				0)).
		r;
}

float4 main(
	PSInput input) : SV_TARGET
{
    const float4 textureColor =
		GrassTexture.Sample(
			GrassSampler,
			input.TexCoord);
	
	//=====================================================
	// Visibility
	//=====================================================

    const float visibility =
	GetGrassVisibility(
		input.WorldPosition);

    if (visibility <
	0.75f)
    {
        discard;
    }

	//=============================================
	// Alpha Clip
	//=============================================
	//
	// Alpha Blendを使わず、
	// 完全透明部分を破棄する。
	//
	// Depthを書けるので大量草向き。
	//=============================================

    clip(
		textureColor.a -
		GrassParameters.w);

	//=============================================
	// Color Variation
	//=============================================

    const float variation =
		lerp(
			0.90f,
			1.08f,
			input.Random);
	
    const float shadow =
	CalculateShadow(
		input.WorldPosition);

    const float3 finalColor =
		textureColor.rgb *
		variation * shadow;

    return float4(
		saturate(
			finalColor),
		1.0f);
}