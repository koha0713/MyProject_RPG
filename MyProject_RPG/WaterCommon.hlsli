#ifndef WATER_COMMON_HLSLI
#define WATER_COMMON_HLSLI

cbuffer WaterConstantBuffer :
    register(b0)
{
    //=============================================
    // Transform
    //=============================================

    float4x4 World;

    float4x4 WorldViewProjection;

    //=============================================
    // Water
    //=============================================

    float4 WaterColor;

    // xy = FlowDirection
    // z  = FlowSpeed
    // w  = Tiling
    float4 FlowA;

    float4 FlowB;

    // x = NormalStrength
    // y = Time
    // z = HasNormalMapA
    // w = HasNormalMapB
    float4 NormalParameters;

    //=============================================
    // Lighting
    //=============================================

    // xyz = Direction
    // w   = Intensity
    float4 LightDirectionIntensity;

    float4 LightColor;

    float4 AmbientLightColor;

    //=============================================
    // Camera
    //=============================================

    // xyz = Camera World Position
    float4 CameraPosition;

    //=============================================
    // Reflection
    //=============================================

    // x = SpecularStrength
    // y = SpecularPower
    // z = FresnelStrength
    // w = FresnelPower
    float4 ReflectionParameters;

    float4 FresnelColor;
    
    //=============================================
    // Water Depth
    //=============================================

    float4 ShallowWaterColor;

    float4 DeepWaterColor;

    // x = ShallowDepth
    // y = DeepDepth
    // z = ShallowAlpha
    // w = DeepAlpha
    float4 DepthParameters;
};

#endif