#include "GrassCommon.hlsli"

struct VSInput
{
    float3 Position : POSITION;

    float2 TexCoord : TEXCOORD;

    float3 InstancePosition :
		INSTANCEPOSITION;

    float InstanceRotation :
		INSTANCEROTATION;

    float InstanceScale :
		INSTANCESCALE;

    float InstanceRandom :
		INSTANCERANDOM;
};

struct VSOutput
{
    float4 Position : SV_POSITION;

    float2 TexCoord : TEXCOORD0;

    float Random : TEXCOORD1;
};

VSOutput main(
	VSInput input)
{
    VSOutput output;

	//=============================================
	// Instance Scale
	//=============================================

    float3 localPosition =
		input.Position *
		input.InstanceScale;

	//=============================================
	// Instance Rotation
	//=============================================

    const float sinRotation =
		sin(
			input.InstanceRotation);

    const float cosRotation =
		cos(
			input.InstanceRotation);

    const float rotatedX =
		localPosition.x *
			cosRotation -
		localPosition.z *
			sinRotation;

    const float rotatedZ =
		localPosition.x *
			sinRotation +
		localPosition.z *
			cosRotation;

    localPosition.x =
		rotatedX;

    localPosition.z =
		rotatedZ;

	//=============================================
	// World Position
	//=============================================

    float3 worldPosition =
		localPosition +
		input.InstancePosition;

	//=============================================
	// Wind
	//=============================================
	//
	// 根元 = 0
	// 先端 = 1
	//
	// Local Yを使うことで、
	// 根元を固定したまま先端だけ揺らす。
	//=============================================

    const float heightFactor =
		saturate(
			input.Position.y);

    const float time =
		GrassParameters.x;

    const float windStrength =
		GrassParameters.y;

    const float windSpeed =
		GrassParameters.z;

    float2 windDirection =
		WindParameters.xy;

    const float directionLength =
		length(
			windDirection);

    if (directionLength >
		0.0001f)
    {
        windDirection /=
			directionLength;
    }

	// 草ごとの位相差
    const float phase =
		input.InstanceRandom *
		6.2831853f;

	// 大きなゆっくりした風
    const float largeWind =
		sin(
			time *
				windSpeed +
			worldPosition.x *
				0.25f +
			worldPosition.z *
				0.18f +
			phase);

	// 細かい揺れ
    const float smallWind =
		sin(
			time *
				windSpeed *
				2.3f +
			worldPosition.x *
				0.75f -
			worldPosition.z *
				0.55f +
			phase *
				1.7f);

    const float wind =
		largeWind *
			0.75f +
		smallWind *
			0.25f;

	// 根元は動かさず、
	// 上へ行くほど揺れを強くする。
    const float bend =
		wind *
		windStrength *
		heightFactor *
		heightFactor;

    worldPosition.x +=
		windDirection.x *
		bend;

    worldPosition.z +=
		windDirection.y *
		bend;

	//=============================================
	// Output
	//=============================================

    output.Position =
		mul(
			float4(
				worldPosition,
				1.0f),
			ViewProjection);

    output.TexCoord =
		input.TexCoord;

    output.Random =
		input.InstanceRandom;

    return output;
}