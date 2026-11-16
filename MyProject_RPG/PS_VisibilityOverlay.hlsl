//=====================================================
// Visibility Overlay Pixel Shader
//=====================================================

cbuffer VisibilityOverlayConstantBuffer :
	register(b0)
{
    float4x4 World;

    float4x4 ViewProjection;

	// x = CellSize
	// y = ParchmentTiling
	// z = HeightOffset
	// w = Time
    float4 Parameters;
};

//=====================================================
// Texture
//=====================================================

// 羊皮紙
Texture2D ParchmentTexture :
	register(t0);

// Visibility
Texture2D VisibilityTexture :
	register(t1);

SamplerState ParchmentSampler :
	register(s0);

/**
 * @brief Visibility境界補間用Sampler
 */
SamplerState VisibilitySampler :
	register(s1);

//=====================================================
// Input
//=====================================================

struct PSInput
{
    float4 Position :
		SV_POSITION;

    float3 WorldPosition :
		TEXCOORD0;
};

//=====================================================
// Visibility
//=====================================================

/**
 * @brief World座標から描画用Visibilityを取得
 *
 * @details
 * Gameplay上はGrid単位だが、
 * Linear Samplingすることで見た目だけ
 * Grid境界を滑らかにする。
 */
float GetSmoothVisibility(
	float3 worldPosition)
{
    const float cellSize =
		max(
			Parameters.x,
			0.0001f);

	//=============================================
	// Texture Size
	//=============================================

    uint width;
    uint height;

    VisibilityTexture.GetDimensions(
		width,
		height);

    const float2 textureSize =
		float2(
			(float) width,
			(float) height);

	//=============================================
	// World → Grid Space
	//=============================================
	//
	// Grid中心:
	// 0 → World 0
	// 1 → World CellSize
	//
	// TextureではTexel中心が
	// (0.5 / Width)なので+0.5する。
	//=============================================

    const float2 gridPosition =
		float2(
			worldPosition.x,
			worldPosition.z) /
		cellSize;

    const float2 visibilityUV =
		(
			gridPosition +
			float2(
				0.5f,
				0.5f)
		) /
		textureSize;

	//=============================================
	// Linear Sample
	//=============================================

    return VisibilityTexture.SampleLevel(
		VisibilitySampler,
		visibilityUV,
		0.0f).r;
}

//=====================================================
// Procedural Noise
//=====================================================

/**
 * @brief 軽量な疑似乱数
 *
 * @details
 * World座標から0～1の疑似乱数を生成する。
 */
float HashNoise(
	float2 p)
{
    p =
		frac(
			p *
			float2(
				123.34f,
				456.21f));

    p +=
		dot(
			p,
			p +
			45.32f);

    return frac(
		p.x *
		p.y);
}

/**
 * @brief 補間付きValue Noise
 *
 * @details
 * 紙粒子やFogの基礎Noiseとして使用する。
 */
float ValueNoise(
	float2 p)
{
    const float2 i =
		floor(
			p);

    const float2 f =
		frac(
			p);

    const float a =
		HashNoise(
			i);

    const float b =
		HashNoise(
			i +
			float2(
				1.0f,
				0.0f));

    const float c =
		HashNoise(
			i +
			float2(
				0.0f,
				1.0f));

    const float d =
		HashNoise(
			i +
			float2(
				1.0f,
				1.0f));

	// Smooth interpolation
    const float2 u =
		f *
		f *
		(
			3.0f -
			2.0f *
			f);

    const float top =
		lerp(
			a,
			b,
			u.x);

    const float bottom =
		lerp(
			c,
			d,
			u.x);

    return lerp(
		top,
		bottom,
		u.y);
}

/**
 * @brief FBM
 *
 * @details
 * 異なる周波数のNoiseを複数重ねることで、
 * 単純な明暗ではなく、
 * 雲・霧のような複雑な模様を生成する。
 */
float FBM(
	float2 p)
{
    float value =
		0.0f;

    float amplitude =
		0.5f;

    float frequency =
		1.0f;

	// 4Octave程度なら見た目と負荷のバランスが良い。
	[unroll]
    for (int i = 0;
		i < 4;
		++i)
    {
        value +=
			ValueNoise(
				p *
				frequency) *
			amplitude;

        frequency *=
			2.0f;

        amplitude *=
			0.5f;
    }

    return value;
}

//=====================================================
// Main
//=====================================================

float4 main(
	PSInput input) : SV_TARGET
{
	//=================================================
	// Visibility
	//=================================================

    const float visibility =
		GetSmoothVisibility(
			input.WorldPosition);

	//=================================================
	// Unexplored Mask
	//=================================================
	//
	// 未探索から探索済みへの境界を
	// smoothstepで滑らかにする。
	//=================================================

    const float parchmentAlpha =
		1.0f -
		smoothstep(
			0.05f,
			0.45f,
			visibility);

    if (parchmentAlpha <=
		0.001f)
    {
        discard;
    }

	//=================================================
	// World Position
	//=================================================

    const float2 worldXZ =
		input.WorldPosition.xz;

	//=================================================
	// Parchment UV
	//=================================================
	//
	// World座標を使用して、
	// Cellごとではなく連続した1枚の紙として表示する。
	//=================================================

    const float2 parchmentUV =
		worldXZ *
		Parameters.y;

	//=================================================
	// Parchment Texture Sample
	//=================================================

    float3 parchmentColor =
		ParchmentTexture.Sample(
			ParchmentSampler,
			parchmentUV).rgb;

	//=================================================
	// Saturation
	//=================================================
	//
	// オレンジ色が強すぎないように
	// グレースケール方向へ寄せる。
	//
	// 0.0 = 完全グレースケール
	// 1.0 = 元画像
	//=================================================

    const float luminance =
		dot(
			parchmentColor,
			float3(
				0.299f,
				0.587f,
				0.114f));

    const float saturation =
		0.40f;

    parchmentColor =
		lerp(
			luminance.xxx,
			parchmentColor,
			saturation);

	//=================================================
	// Contrast
	//=================================================
	//
	// 元画像の模様の主張を弱める。
	//=================================================

    const float contrast =
		0.70f;

    parchmentColor =
		(
			parchmentColor -
			0.5f
		) *
		contrast +
		0.5f;

	//=================================================
	// Brightness
	//=================================================

    const float brightness =
		1.15f;

    parchmentColor *=
		brightness;

	//=================================================
	// Beige Tint
	//=================================================
	//
	// 赤・オレンジ感を抑えて
	// 淡いベージュへ寄せる。
	//=================================================

    const float3 parchmentTint =
		float3(
			0.95f,
			0.90f,
			0.80f);

    parchmentColor *=
		parchmentTint;

	//=================================================
	// Paper Grain
	//=================================================
	//
	// World座標基準の細かなNoiseを加えて、
	// 紙表面の細かな粒状感を出す。
	//
	// Cameraを動かしても模様はWorld上に固定される。
	//=================================================

    const float paperNoise =
		ValueNoise(
			worldXZ *
			7.0f);

    const float paperGrain =
		(
			paperNoise -
			0.5f
		) *
		0.02f;

    parchmentColor +=
		paperGrain;

	//=================================================
	// Moving Fog
	//=================================================
	//
	// 単一Noiseではなく、
	// FBMを2レイヤー異なる方向へ流す。
	//
	// これによって単純な明暗変化ではなく、
	// 薄いFogが漂っているような動きを作る。
	//=================================================

    const float time =
		Parameters.w;

	//=============================================
	// Fog Layer 1
	//=============================================
	//
	// 大きめでゆっくり流れるFog。
	//=============================================

    const float2 fogUV1 =
		worldXZ *
		0.10f +
		float2(
			time *
				0.15f,
			time *
				0.07f);

    const float fog1 =
		FBM(
			fogUV1);

	//=============================================
	// Fog Layer 2
	//=============================================
	//
	// 少し細かく、
	// Layer1とは異なる方向へ流す。
	//=============================================

    const float2 fogUV2 =
		worldXZ *
		0.18f +
		float2(
			-time *
				0.010f,
			time *
				0.012f);

    const float fog2 =
		FBM(
			fogUV2);

	//=============================================
	// Fog Combine
	//=============================================

    float fog =
		fog1 *
		0.65f +
		fog2 *
		0.35f;

	//=============================================
	// Fog Density
	//=============================================
	//
	// FBM値をそのまま使わず、
	// smoothstepで霧の塊を形成する。
	//=============================================

    fog =
		smoothstep(
			0.35f,
			0.75f,
			fog);

	//=================================================
	// Fog Color
	//=================================================
	//
	// 単純に明るさを加算するのではなく、
	// Fogがある部分だけ淡いベージュへ寄せる。
	//
	// これにより、
	// 「明るい・暗い」ではなく
	// 「薄い霧が被っている」見た目になる。
	//=================================================

    const float3 fogColor =
		float3(
			0.96f,
			0.94f,
			0.88f);

    const float fogStrength =
		0.8f;

    parchmentColor =
		lerp(
			parchmentColor,
			fogColor,
			fog *
			fogStrength);

	//=================================================
	// Fog Alpha
	//=================================================
	//
	// Fogが濃い場所だけOverlayを
	// わずかに濃くする。
	//
	// 強くすると煙のようになるため、
	// 非常に小さな値にする。
	//=================================================

    const float fogAlphaStrength =
		1.0f;

    const float finalAlpha =
		saturate(
			parchmentAlpha +
			fog *
			fogAlphaStrength);

	//=================================================
	// Final
	//=================================================

    return float4(
		saturate(
			parchmentColor),
		finalAlpha);
}