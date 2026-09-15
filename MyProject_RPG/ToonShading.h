#pragma once

/**
 * @brief Toon Shading設定
 *
 * @details
 * Directional LightによるNdotLを段階化し、
 * アニメ・イラスト調の陰影を作るための設定。
 */
struct ToonShading
{
	/**
	 * @brief 明部と中間部の境界
	 */
	float HighlightThreshold = 0.80f;

	/**
	 * @brief 中間部と影部の境界
	 */
	float ShadowThreshold = 0.55f;

	/**
	 * @brief 中間部の明るさ
	 */
	float MidToneIntensity = 0.70f;

	/**
	 * @brief 影部分の明るさ
	 */
	float ShadowIntensity = 0.40f;
};