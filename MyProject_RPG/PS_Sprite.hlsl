Texture2D SpriteTexture : register(t0);
SamplerState SpriteSampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

// LinearからsRGBへ変換する
float3 LinearToSRGB(float3 color)
{
    color = saturate(color);

    float3 low = color * 12.92f;

    float3 high =
        1.055f * pow(color, 1.0f / 2.4f) - 0.055f;

    return lerp(
        low,
        high,
        step(0.0031308f, color));
}

float4 main(PSInput input) : SV_TARGET
{
    // sRGB SRVの場合、サンプリング結果はLinear RGB
    float4 color =
        SpriteTexture.Sample(
            SpriteSampler,
            input.TexCoord);

    // 通常の色乗算
    color *= input.Color;

    // 検証用：UNORM RTVへ出す前にsRGBへ戻す
    color.rgb = LinearToSRGB(color.rgb);

    // Alphaはガンマ変換しない
    return color;
}