#include "WaterCommon.hlsli"

Texture2D NormalMapA :
    register(t0);

Texture2D NormalMapB :
    register(t1);

SamplerState WaterSampler :
    register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;

    float3 WorldPosition : TEXCOORD0;

    /**
     * Vertex Shaderから補間された水深
     */
    float WaterDepth : TEXCOORD1;
};

/**
 * @brief Normal MapのRGB値を
 *        [-1, 1]へ変換する
 */
float3 DecodeNormal(
    float3 encodedNormal)
{
    return
        encodedNormal *
        2.0f -
        1.0f;
}

float4 main(
    PSInput input) : SV_TARGET
{
    //=============================================
    // Parameters
    //=============================================

    const float time =
        NormalParameters.y;

    const float specularStrength =
        ReflectionParameters.x;

    const float specularPower =
        ReflectionParameters.y;

    const float fresnelStrength =
        ReflectionParameters.z;

    const float fresnelPower =
        ReflectionParameters.w;

    //=============================================
    // World Space UV
    //=============================================
    //
    // ChunkやGrid境界に依存せず、
    // World座標から連続したUVを生成する。
    //=============================================

    const float2 worldUV =
        input.WorldPosition.xz;

    //=============================================
    // Normal Map A
    //=============================================

    float2 uvA =
        worldUV *
        FlowA.w;

    uvA +=
        FlowA.xy *
        FlowA.z *
        time;

    //=============================================
    // Normal Map B
    //=============================================

    float2 uvB =
        worldUV *
        FlowB.w;

    uvB +=
        FlowB.xy *
        FlowB.z *
        time;

    //=============================================
    // Normal Sampling
    //=============================================

    float3 normalA =
        float3(
            0.0f,
            0.0f,
            1.0f);

    float3 normalB =
        float3(
            0.0f,
            0.0f,
            1.0f);

    if (NormalParameters.z >
        0.5f)
    {
        normalA =
            DecodeNormal(
                NormalMapA.Sample(
                    WaterSampler,
                    uvA).xyz);
    }

    if (NormalParameters.w >
        0.5f)
    {
        normalB =
            DecodeNormal(
                NormalMapB.Sample(
                    WaterSampler,
                    uvB).xyz);
    }

    //=============================================
    // Combine Normal Maps
    //=============================================

    float3 tangentNormal =
        normalize(
            float3(
                normalA.x +
                    normalB.x,

                normalA.y +
                    normalB.y,

                normalA.z *
                    normalB.z));

    // Normal Mapの横方向だけ強度を変更する
    tangentNormal.xy *=
        NormalParameters.x;

    tangentNormal =
        normalize(
            tangentNormal);

    //=============================================
    // Tangent → World
    //=============================================
    //
    // 水面はXZ平面固定。
    //
    // Tangent X = World X
    // Tangent Y = World Z
    // Tangent Z = World Y
    //=============================================

    const float3 worldNormal =
        normalize(
            float3(
                tangentNormal.x,
                tangentNormal.z,
                tangentNormal.y));
    
    //=============================================
    // Water Depth
    //=============================================

    const float shallowDepth =
    DepthParameters.x;

    const float deepDepth =
    DepthParameters.y;

    const float shallowAlpha =
    DepthParameters.z;

    const float deepAlpha =
    DepthParameters.w;

    // 0 = shallow
    // 1 = deep
    const float depthRange =
    max(
        deepDepth -
        shallowDepth,
        0.0001f);

    const float depthFactor =
    saturate(
        (
            input.WaterDepth -
            shallowDepth
        ) /
        depthRange);

    //=============================================
    // Depth Color
    //=============================================
    //
    // 浅い場所
    // → 明るいシアン
    //
    // 深い場所
    // → 濃い青
    //=============================================

    const float3 depthWaterColor =
    lerp(
        ShallowWaterColor.rgb,
        DeepWaterColor.rgb,
        depthFactor);

    //=============================================
    // Depth Alpha
    //=============================================

    const float waterAlpha =
    lerp(
        shallowAlpha,
        deepAlpha,
        depthFactor);

    //=============================================
    // Light Direction
    //=============================================

    const float3 lightDirection =
        normalize(
            -LightDirectionIntensity.xyz);

    //=============================================
    // View Direction
    //=============================================
    //
    // 水面Pixel → Camera
    //=============================================

    const float3 viewDirection =
        normalize(
            CameraPosition.xyz -
            input.WorldPosition);

    //=============================================
    // Half-Lambert
    //=============================================

    const float rawNdotL =
        dot(
            worldNormal,
            lightDirection);

    const float halfLambert =
        saturate(
            rawNdotL *
                0.5f +
            0.5f);

    // 水面が黒くなりすぎないよう
    // 最低照度を確保する
    const float waterLighting =
        lerp(
            0.65f,
            1.0f,
            halfLambert);

    //=============================================
    // Base Lighting
    //=============================================

    const float3 diffuse =
        LightColor.rgb *
        LightDirectionIntensity.w *
        waterLighting;

    const float3 ambient =
        AmbientLightColor.rgb;

    float3 finalColor =
        depthWaterColor *
        saturate(
            ambient +
            diffuse);

    //=============================================
    // Blinn-Phong Specular
    //=============================================
    //
    // LightとViewの中間ベクトルを使用する。
    //
    // 波Normalが適切な方向を向いた場所だけ
    // 太陽光反射が現れる。
    //=============================================

    const float3 halfVector =
        normalize(
            lightDirection +
            viewDirection);

    const float specularNdotH =
        saturate(
            dot(
                worldNormal,
                halfVector));

    const float specular =
        pow(
            specularNdotH,
            specularPower) *
        specularStrength *
        LightDirectionIntensity.w;

    // 太陽光に少し暖色を付ける
    const float3 sunSpecularColor =
        LightColor.rgb *
        float3(
            1.00f,
            0.96f,
            0.88f);

    finalColor +=
        sunSpecularColor *
        specular;

    //=============================================
    // Fresnel
    //=============================================
    //
    // 水面を正面から見る
    // → 弱い
    //
    // 水面を斜めから見る
    // → 強い
    //
    //=============================================

    const float NdotV =
        saturate(
            dot(
                worldNormal,
                viewDirection));

    const float fresnel =
        pow(
            1.0f -
            NdotV,
            fresnelPower) *
        fresnelStrength;

    // Fresnelは完全加算ではなく、
    // 反射色へ補間する。
    finalColor =
        lerp(
            finalColor,
            FresnelColor.rgb,
            saturate(
                fresnel));

    //=============================================
    // Final
    //=============================================
    return float4(
        saturate(
            finalColor),
        waterAlpha);
}