#pragma once

#include "CommonType.h"

#include <cstdint>

/**
 * @brief Terrainê—pVertex
 *
 * Terrain Shader‚ÖTerrainí—Ş‚ğ“n‚·‚½‚ßA
 * ’Êí‚ÌVertexData‚Æ‚Í•ª—£‚·‚éB
 */
struct TerrainVertex
{
	Vector3 Position =
		Vector3(
			0.0f,
			0.0f,
			0.0f);

	Vector3 Normal =
		Vector3(
			0.0f,
			1.0f,
			0.0f);

	Vector2 TexCoord =
		Vector2(
			0.0f,
			0.0f);

	/**
	 * @brief Terrain Texture”Ô†
	 *
	 * 0 = Grass
	 * 1 = Dirt
	 * 2 = Rock
	 * 3 = Sand
	 */
	float TerrainIndex =
		0;

	/**
	 * @brief üˆÍ4•ûŒü‚ÌTerrain”Ô†
	 *
	 * x = Up
	 * y = Right
	 * z = Down
	 * w = Left
	 */
	Vector4 NeighborTerrainIndices =
		Vector4(
			0.0f,
			0.0f,
			0.0f,
			0.0f);
};