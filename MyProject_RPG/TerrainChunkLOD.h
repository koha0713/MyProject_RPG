#pragma once

/**
 * @brief Terrain Chunkの描画LOD
 */
enum class TerrainChunkLOD
{
	/**
	 * @brief Camera近距離
	 *
	 * Grass高密度、
	 * Water高品質など。
	 */
	Near,

	/**
	 * @brief Camera中距離
	 */
	Middle,

	/**
	 * @brief Camera遠距離
	 *
	 * Grassを描画しない等。
	 */
	Far,

	/**
	 * @brief 描画対象外
	 */
	Culled
};