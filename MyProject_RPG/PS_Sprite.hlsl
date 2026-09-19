Texture2D SpriteTexture :
	register(t0);

SamplerState SpriteSampler :
	register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

float4 main(
	PSInput input) : SV_TARGET
{
    float4 textureColor =
		SpriteTexture.Sample(
			SpriteSampler,
			input.TexCoord);

    return
		textureColor *
		input.Color;
}