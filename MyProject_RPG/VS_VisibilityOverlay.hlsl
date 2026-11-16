//=====================================================
// Visibility Overlay Vertex Shader
//=====================================================

cbuffer VisibilityOverlayConstantBuffer :
	register(b0)
{
	// Local Å® World
    float4x4 World;

	// World Å® View Å® Projection
    float4x4 ViewProjection;

	// x = CellSize
	// y = ParchmentTiling
	// z = HeightOffset
	// w = unused
    float4 Parameters;
};

struct VSInput
{
    float3 Position :
		POSITION;
};

struct VSOutput
{
    float4 Position :
		SV_POSITION;

    float3 WorldPosition :
		TEXCOORD0;
};

VSOutput main(
	VSInput input)
{
    VSOutput output;

	//=================================================
	// Local Å® World
	//=================================================

    float4 worldPosition =
		mul(
			float4(
				input.Position,
				1.0f),
			World);

	//=================================================
	// Overlay Offset
	//=================================================
	//
	// å≥TerrainÇ∆ìØÇ∂Depthà íuÇæÇ∆
	// Z-FightingÇ™î≠ê∂Ç∑ÇÈÇΩÇﬂè≠Çµè„Ç÷éùÇøè„Ç∞ÇÈÅB
	//=================================================

    worldPosition.y +=
		Parameters.z;

    output.WorldPosition =
		worldPosition.xyz;

	//=================================================
	// World Å® Screen
	//=================================================

    output.Position =
		mul(
			worldPosition,
			ViewProjection);

    return output;
}