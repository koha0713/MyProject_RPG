#ifndef SHADOW_COMMON_HLSLI
#define SHADOW_COMMON_HLSLI

//=====================================================
// Shadow Map
//=====================================================

Texture2D ShadowMap :
	register(t7);

/**
 * @brief Shadow Map比較専用Sampler
 *
 * 通常のSamplerStateではなく、
 * Depth比較を行うSamplerComparisonStateを使用する。
 */
SamplerComparisonState ShadowSampler :
	register(s7);


//=====================================================
// Shadow Constant Buffer
//=====================================================

cbuffer ShadowReceiveConstantBuffer :
	register(b2)
{
    float4x4 LightViewProjection;

	// x = Bias
	// y = ShadowStrength
	// z = ShadowMapSize
	// w = unused
    float4 ShadowParameters;
};


//=====================================================
// Calculate Shadow
//=====================================================

float CalculateShadow(
	float3 worldPosition)
{
	//=============================================
	// World → Light Clip Space
	//=============================================

    const float4 lightPosition =
		mul(
			float4(
				worldPosition,
				1.0f),
			LightViewProjection);

	//=============================================
	// Perspective Divide
	//=============================================

    const float inverseW =
		1.0f /
		max(
			lightPosition.w,
			0.0001f);

    const float3 projected =
		lightPosition.xyz *
		inverseW;

	//=============================================
	// Depth Range Check
	//=============================================

    if (projected.z < 0.0f ||
		projected.z > 1.0f)
    {
        return 1.0f;
    }

	//=============================================
	// NDC → Texture UV
	//=============================================

    const float2 uv =
		float2(
			projected.x *
				0.5f +
			0.5f,

			-projected.y *
				0.5f +
			0.5f);

    if (uv.x < 0.0f ||
		uv.x > 1.0f ||
		uv.y < 0.0f ||
		uv.y > 1.0f)
    {
        return 1.0f;
    }

	//=============================================
	// Parameters
	//=============================================

    const float bias =
		ShadowParameters.x;

    const float strength =
		ShadowParameters.y;

    const float mapSize =
		max(
			ShadowParameters.z,
			1.0f);

    const float2 texelSize =
		float2(
			1.0f / mapSize,
			1.0f / mapSize);

	//=============================================
	// PCF 3x3
	//=============================================

    float visibility =
		0.0f;

	[unroll]
    for (int y = -1;
		y <= 1;
		++y)
    {
		[unroll]
        for (int x = -1;
			x <= 1;
			++x)
        {
            const float2 sampleUV =
				uv +
				float2(
					(float) x,
					(float) y) *
				texelSize;

            visibility +=
				ShadowMap.
				SampleCmpLevelZero(
					ShadowSampler,
					sampleUV,
					projected.z -
						bias);
        }
    }

    visibility /=
		9.0f;

    return
		lerp(
			1.0f -
				strength,
			1.0f,
			visibility);
}

#endif