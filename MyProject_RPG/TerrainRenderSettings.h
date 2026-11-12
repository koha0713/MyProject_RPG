#pragma once

#include "CommonType.h"

/**
 * @brief Terrain Shader—p•`‰æİ’è
 */
struct TerrainRenderSettings
{
	/**
	 * @brief World Space UV‚ÌŠg‘å—¦
	 *
	 * ¬‚³‚¢‚Ù‚ÇTexture‚ª‘å‚«‚­Œ©‚¦‚éB
	 */
	float Tiling =
		0.35f;

	/**
	 * @brief Terrain‘S‘Ì‚ÖŠ|‚¯‚éF
	 */
	Vector4 Color =
		Vector4(
			1.0f,
			1.0f,
			1.0f,
			1.0f);
};