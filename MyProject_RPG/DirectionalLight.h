#pragma once

#include "CommonType.h"

/**
 * @brief 平行光源
 */
struct DirectionalLight
{
	/**
	 * @brief 光が進む方向
	 * Rendererへ設定するときに正規化される。
	 */
	Vector3 Direction =
		Vector3(
			0.0f,
			-1.0f,
			0.0f);

	/**
	 * @brief 光源色
	 */
	Color LightColor =
		Color(
			1.0f,
			1.0f,
			1.0f,
			1.0f);

	/**
	 * @brief 光の強さ
	 */
	float Intensity = 1.0f;
};