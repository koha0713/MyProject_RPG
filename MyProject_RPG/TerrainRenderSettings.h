#pragma once

#include "CommonType.h"

/**
 * @brief Terrain Shader用描画設定
 */
struct TerrainRenderSettings
{
	/**
	 * @brief World Space UVの拡大率
	 *
	 * 小さいほどTextureが大きく見える。
	 */
	float Tiling =
		0.35f;

	/**
	 * @brief Grid 1マスのWorldサイズ
	 *
	 * VisibilityTextureのGrid座標計算に使用する。
	 */
	float CellSize =
		1.0f;

	/**
	 * @brief 探索済み・現在視界外の明るさ
	 */
	float ExploredBrightness =
		0.55f;

	/**
	 * @brief 羊皮紙TextureのTiling
	 */
	float ParchmentTiling =
		0.12f;

	/**
	 * @brief Terrain全体へ掛ける色
	 */
	Vector4 Color =
		Vector4(
			1.0f,
			1.0f,
			1.0f,
			1.0f);
};