#include "TerrainCommon.hlsli"
#include "ShadowCommon.hlsli"

Texture2D GrassTexture :
	register(t0);

Texture2D DirtTexture :
	register(t1);

Texture2D RockTexture :
	register(t2);

Texture2D SandTexture :
	register(t3);

Texture2D VisibilityTexture :
	register(t4);

SamplerState TerrainSampler :
	register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;

    float3 WorldPosition : TEXCOORD0;

    float3 WorldNormal : TEXCOORD1;

    float2 LocalUV : TEXCOORD2;

	// x = Center
	// y = Up
	// z = Right
	// w = Down
    float4 TerrainInfo : TEXCOORD3;

	// x = Left
    float4 TerrainInfo2 : TEXCOORD4;
};


//=====================================================
// Sample Terrain Texture
//=====================================================

float4 SampleTerrainTexture(
	float terrainIndex,
	float2 uv)
{
    if (terrainIndex < 0.5f)
    {
        return
			GrassTexture.Sample(
				TerrainSampler,
				uv);
    }

    if (terrainIndex < 1.5f)
    {
        return
			DirtTexture.Sample(
				TerrainSampler,
				uv);
    }

    if (terrainIndex < 2.5f)
    {
        return
			RockTexture.Sample(
				TerrainSampler,
				uv);
    }

    return
		SandTexture.Sample(
			TerrainSampler,
			uv);
}

/**
 * @brief World座標から固定Noiseを生成
 *
 * @return 0.0 ～ 1.0
 */
float HashNoise(
	float2 p)
{
    const float value =
		sin(
			dot(
				p,
				float2(
					12.9898f,
					78.233f))) *
		43758.5453f;

    return frac(value);
}

/**
 * @brief World座標からGridのVisibility値を取得
 */
float GetVisibility(
	float3 worldPosition)
{
	//=============================================
	// Cell Size
	//=============================================

    const float cellSize =
		max(
			TerrainParameters.y,
			0.0001f);

	//=============================================
	// World → Grid
	//=============================================
	//
	// Grid中心:
	//
	// grid 0 → world 0
	// grid 1 → world cellSize
	//
	// Cell境界が±0.5なので、
	// +0.5してfloorする。
	//=============================================

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

	//=============================================
	// Texture Size
	//=============================================

    uint width;
    uint height;

    VisibilityTexture.
		GetDimensions(
			width,
			height);

	// Map外は未探索
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

//=====================================================
// Smooth Visibility
//=====================================================
//
// VisibilityTextureの4つのTexelを手動で線形補間する。
// SamplerStateを追加しないため、C++側の変更は不要。
//
// Grid中心:
//   Grid(0, 0) = World(0, 0)
//   Grid(1, 0) = World(CellSize, 0)
//
// VisibilityTexture:
//   Unexplored = 0.0
//   Explored   ≒ 0.5
//   Visible    = 1.0
//=====================================================

float GetSmoothVisibility(
	float3 worldPosition)
{
    const float cellSize =
		max(
			TerrainParameters.y,
			0.0001f);

	//=============================================
	// Texture Size
	//=============================================

    uint width;
    uint height;

    VisibilityTexture.GetDimensions(
		width,
		height);

	//=============================================
	// World → Grid Space
	//=============================================

    const float2 gridPosition =
		float2(
			worldPosition.x,
			worldPosition.z) /
		cellSize;

	// 現在位置を囲んでいる左上側のGrid
    const int2 baseGrid =
		int2(
			floor(
				gridPosition));

	// Grid内での補間率
    const float2 fraction =
		frac(
			gridPosition);

	//=============================================
	// Grid Clamp
	//=============================================

    const int2 maxGrid =
		int2(
			(int) width - 1,
			(int) height - 1);

    const int2 p00 =
		clamp(
			baseGrid,
			int2(0, 0),
			maxGrid);

    const int2 p10 =
		clamp(
			baseGrid +
			int2(1, 0),
			int2(0, 0),
			maxGrid);

    const int2 p01 =
		clamp(
			baseGrid +
			int2(0, 1),
			int2(0, 0),
			maxGrid);

    const int2 p11 =
		clamp(
			baseGrid +
			int2(1, 1),
			int2(0, 0),
			maxGrid);

	//=============================================
	// 4 Texel Load
	//=============================================

    const float v00 =
		VisibilityTexture.Load(
			int3(
				p00,
				0)).r;

    const float v10 =
		VisibilityTexture.Load(
			int3(
				p10,
				0)).r;

    const float v01 =
		VisibilityTexture.Load(
			int3(
				p01,
				0)).r;

    const float v11 =
		VisibilityTexture.Load(
			int3(
				p11,
				0)).r;

	//=============================================
	// Bilinear Interpolation
	//=============================================

    const float top =
		lerp(
			v00,
			v10,
			fraction.x);

    const float bottom =
		lerp(
			v01,
			v11,
			fraction.x);

    return lerp(
		top,
		bottom,
		fraction.y);
}

//=====================================================
// Main
//=====================================================

float4 main(
	PSInput input) : SV_TARGET
{
	//=================================================
	// Terrain Information
	//=================================================

    const float centerTerrain =
		input.TerrainInfo.x;

    const float terrainUp =
		input.TerrainInfo.y;

    const float terrainRight =
		input.TerrainInfo.z;

    const float terrainDown =
		input.TerrainInfo.w;

    const float terrainLeft =
		input.TerrainInfo2.x;

	//=================================================
	// World Space UV
	//=================================================

    const float2 terrainUV =
		input.WorldPosition.xz *
		TerrainParameters.x;

	//=================================================
	// Base Terrain
	//=================================================

    float4 terrainColor =
		SampleTerrainTexture(
			centerTerrain,
			terrainUV);
	
	//=====================================================
	// Border Noise
	//=====================================================
	//
	// World座標ベースなのでChunkやCellをまたいでも
	// 同じ位置では同じNoise値になる。
	//=====================================================

		const float noise =
		HashNoise(
			floor(
				input.WorldPosition.xz *
				3.0f));

	// -1 ～ +1
		const float signedNoise =
		noise *
		2.0f -
		1.0f;

	// 境界を揺らす量。
	// 大きすぎるとギザギザしすぎるため小さめ。
    const float BORDER_NOISE_STRENGTH =
	0.035f;

    const float borderOffset =
	signedNoise *
	BORDER_NOISE_STRENGTH;

	//=================================================
	// Border Blend
	//=================================================
	//
	// 1Cell全体をBlendするのではなく、
	// Cell端から18%程度だけを使用する。
	//
	// さらに共有境界では最大0.5までしか混ぜない。
	//
	// これにより、
	//
	// Grass側 = Grass50% + Dirt50%
	// Dirt側  = Dirt50%  + Grass50%
	//
	// となり境界の色が一致する。
	//=================================================

    const float BLEND_WIDTH =
		0.18f;

	//=================================================
	// Up
	//=================================================

    if (abs(
		terrainUp -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				BLEND_WIDTH +
					borderOffset,

				0.0f +
					borderOffset,

				input.LocalUV.y) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainUp,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Right
	//=================================================

    if (abs(
		terrainRight -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				1.0f -
					BLEND_WIDTH +
					borderOffset,

				1.0f +
					borderOffset,

				input.LocalUV.x) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainRight,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Down
	//=================================================

    if (abs(
		terrainDown -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				1.0f -
					BLEND_WIDTH +
					borderOffset,

				1.0f +
					borderOffset,

				input.LocalUV.y) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainDown,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Left
	//=================================================

    if (abs(
		terrainLeft -
		centerTerrain) >
		0.1f)
    {
        const float blend =
			smoothstep(
				BLEND_WIDTH +
					borderOffset,

				0.0f +
					borderOffset,

				input.LocalUV.x) *
			0.5f;

        const float4 neighborColor =
			SampleTerrainTexture(
				terrainLeft,
				terrainUV);

        terrainColor =
			lerp(
				terrainColor,
				neighborColor,
				blend);
    }

	//=================================================
	// Lighting
	//=================================================

    const float3 normal =
		normalize(
			input.WorldNormal);

    const float3 lightDirection =
		normalize(
			-LightDirectionIntensity.xyz);

    const float ndotl =
		saturate(
			dot(
				normal,
				lightDirection));

	//=================================================
	// Toon
	//=================================================

    float toonIntensity =
		1.0f;

    if (ndotl >=
		ToonParameters.x)
    {
        toonIntensity =
			1.0f;
    }
    else if (ndotl >=
		ToonParameters.y)
    {
        toonIntensity =
			ToonParameters.z;
    }
    else
    {
        toonIntensity =
			ToonParameters.w;
    }

	//=================================================
	// Lighting Color
	//=================================================

    const float3 directional =
		LightColor.rgb *
		LightDirectionIntensity.w *
		toonIntensity;
	
    const float shadow =
	CalculateShadow(
		input.WorldPosition);

    const float3 lighting =
		AmbientLightColor.rgb +
		directional * shadow;

//=====================================================
// Final Terrain Color
//=====================================================

    float3 finalColor =
	terrainColor.rgb *
	TerrainColor.rgb *
	lighting;

//=====================================================
// Smooth Visibility
//=====================================================

    const float visibility =
	GetSmoothVisibility(
		input.WorldPosition);

//=====================================================
// Visible Blend
//=====================================================
//
// Explored ≒ 0.5
// Visible  = 1.0
//
// 0.5 → 1.0の範囲を滑らかに
// 「暗い探索済みTerrain」→「現在見えているTerrain」
// へBlendする。
//=====================================================

    const float visibleBlend =
	smoothstep(
		0.50f,
		0.90f,
		visibility);

//=====================================================
// Brightness
//=====================================================
//
// visibleBlend = 0
// → ExploredBrightness
//
// visibleBlend = 1
// → 通常の明るさ
//=====================================================

    const float visibilityBrightness =
	lerp(
		TerrainParameters.z,
		1.0f,
		visibleBlend);

    finalColor *=
	visibilityBrightness;

    return float4(
	saturate(
		finalColor),
	1.0f);

//=====================================================
// Visible
//=====================================================
//
// Visibleの場合は通常の明るさ。
//=====================================================

    return float4(
	saturate(
		finalColor),
	1.0f);
}