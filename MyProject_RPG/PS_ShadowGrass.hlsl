#include "GrassCommon.hlsli"

Texture2D GrassTexture :
	register(t0);

SamplerState GrassSampler :
	register(s0);

struct PSInput
{
    float4 Position :
		SV_POSITION;

    float2 TexCoord :
		TEXCOORD0;
};

void main(
	PSInput input)
{
    const float alpha =
		GrassTexture.Sample(
			GrassSampler,
			input.TexCoord).a;

	// ‘Texture‚Ì“§–¾•”•ª‚Í
	// Shadow Map‚É‚à‘‚«‚Ü‚È‚¢
    clip(
		alpha -
		GrassParameters.w);
}