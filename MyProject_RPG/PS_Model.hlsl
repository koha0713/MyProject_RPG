//=====================================================
// Model Pixel Shader
//=====================================================

cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 WorldViewProjection;

    float4x4 WorldInverseTranspose;

    float4 DiffuseColor;

    // xyz : Œõ‚ªi‚Þ•ûŒü
    // w   : Light Intensity
    float4 LightDirectionIntensity;

    float4 LightColor;

    float4 AmbientLightColor;

    // x : HighlightThreshold
    // y : ShadowThreshold
    // z : MidToneIntensity
    // w : ShadowIntensity
    float4 ToonParameters;

    uint HasTexture;

    float3 Padding;
};

//=====================================================
// Texture
//=====================================================

Texture2D ModelTexture :
    register(t0);

SamplerState ModelSampler :
    register(s0);

//=====================================================
// Input
//=====================================================

struct PSInput
{
    float4 Position : SV_POSITION;

    float3 WorldNormal : NORMAL;

    float2 TexCoord : TEXCOORD0;
};

//=====================================================
// Main
//=====================================================

float4 main(
    PSInput input) : SV_TARGET
{
    //=================================================
    // Base Color
    //=================================================

    float4 baseColor =
        DiffuseColor;

    if (HasTexture != 0)
    {
        const float4 textureColor =
            ModelTexture.Sample(
                ModelSampler,
                input.TexCoord);

        baseColor *=
            textureColor;
    }

    //=================================================
    // Normal
    //=================================================

    const float3 normal =
        normalize(
            input.WorldNormal);

    //=================================================
    // Directional Light
    //=================================================

    // Direction‚ÍuŒõ‚ªi‚Þ•ûŒüv‚È‚Ì‚Å”½“]‚·‚é
    const float3 lightDirection =
        normalize(
            -LightDirectionIntensity.xyz);

    const float lightIntensity =
        LightDirectionIntensity.w;

    //=================================================
    // N dot L
    //=================================================

    const float NdotL =
        saturate(
            dot(
                normal,
                lightDirection));

    //=================================================
    // Toon Shading Parameters
    //=================================================

    const float highlightThreshold =
        ToonParameters.x;

    const float shadowThreshold =
        ToonParameters.y;

    const float midToneIntensity =
        ToonParameters.z;

    const float shadowIntensity =
        ToonParameters.w;

    //=================================================
    // 3’iŠK Toon Shading
    //=================================================

    float toonIntensity =
        1.0f;

    if (NdotL >= highlightThreshold)
    {
        // –¾•”
        toonIntensity =
            1.0f;
    }
    else if (NdotL >= shadowThreshold)
    {
        // ’†ŠÔ•”
        toonIntensity =
            midToneIntensity;
    }
    else
    {
        // ‰e•”
        toonIntensity =
            shadowIntensity;
    }

    //=================================================
    // Directional Light
    //=================================================

    const float3 toonLight =
        LightColor.rgb *
        lightIntensity *
        toonIntensity;

    //=================================================
    // Ambient
    //=================================================

    const float3 ambientLight =
        AmbientLightColor.rgb;

    //=================================================
    // Final Lighting
    //=================================================

    const float3 lighting =
        ambientLight +
        toonLight;

    const float3 finalColor =
        baseColor.rgb *
        lighting;

    const float3 gammaCorrected =
    pow(
        saturate(finalColor),
        1.0f / 2.2f);

    return float4(
    gammaCorrected,
    baseColor.a);
}