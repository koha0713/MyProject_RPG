#pragma once

#include "CommonType.h"

/**
 * @brief Toon Outline設定
 */
struct OutlineSetting
{
	/**
	 * @brief アウトラインの太さ
	 */
	float Width = 0.015f;

	/**
	 * @brief アウトライン色
	 */
	Color OutlineColor =
		Color(
			0.02f,
			0.02f,
			0.02f,
			1.0f);
};