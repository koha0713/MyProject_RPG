#include "GrassCommon.hlsli"

Texture2D GrassTexture :
	register(t0);

SamplerState GrassSampler :
	register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;

    float2 TexCoord : TEXCOORD0;

    float Random : TEXCOORD1;
};

float4 main(
	PSInput input) : SV_TARGET
{
    const float4 textureColor =
		GrassTexture.Sample(
			GrassSampler,
			input.TexCoord);

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

    const float3 finalColor =
		textureColor.rgb *
		variation;

    return float4(
		saturate(
			finalColor),
		1.0f);
}