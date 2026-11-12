#pragma once

#include "CommonType.h"

/**
 * @brief 水面描画設定
 */
struct WaterRenderSettings
{
	//=================================================
	// Base Color
	//=================================================

	/**
	 * @brief 水面の基本色
	 */
	Vector4 Color =
		Vector4(
			0.22f,
			0.68f,
			0.90f,
			0.60f);

	//=================================================
	// Normal Map A
	//=================================================

	Vector2 FlowDirectionA =
		Vector2(
			1.0f,
			0.10f);

	float FlowSpeedA =
		0.15f;

	float TilingA =
		0.35f;

	//=================================================
	// Normal Map B
	//=================================================

	Vector2 FlowDirectionB =
		Vector2(
			-0.20f,
			0.95f);

	float FlowSpeedB =
		0.08f;

	float TilingB =
		0.55f;

	//=================================================
	// Normal
	//=================================================

	/**
	 * @brief Normal Mapによる波の強さ
	 */
	float NormalStrength =
		0.14f;

	//=================================================
	// Specular
	//=================================================

	/**
	 * @brief 太陽光反射の強さ
	 */
	float SpecularStrength =
		1.00f;

	/**
	 * @brief 太陽光反射の鋭さ
	 *
	 * 大きいほど細く強いハイライトになる。
	 */
	float SpecularPower =
		160.0f;

	//=================================================
	// Fresnel
	//=================================================

	/**
	 * @brief 斜めから見たときの反射強度
	 */
	float FresnelStrength =
		0.35f;

	/**
	 * @brief Fresnelの立ち上がり方
	 */
	float FresnelPower =
		4.0f;

	/**
	 * @brief Fresnel反射色
	 */
	Vector4 FresnelColor =
		Vector4(
			0.65f,
			0.85f,
			1.0f,
			1.0f);

	//=====================================================
	// Depth Color
	//=====================================================

	/**
	 * @brief 浅い場所の水色
	 */
	Vector4 ShallowColor =
		Vector4(
			0.32f,
			0.78f,
			0.92f,
			1.0f);

	/**
	 * @brief 深い場所の水色
	 */
	Vector4 DeepColor =
		Vector4(
			0.08f,
			0.38f,
			0.68f,
			1.0f);

	/**
	 * @brief この深さ以下を浅瀬として扱う
	 */
	float ShallowDepth =
		0.08f;

	/**
	 * @brief この深さ以上を深場として扱う
	 */
	float DeepDepth =
		0.45f;

	/**
	 * @brief 浅瀬の透明度
	 */
	float ShallowAlpha =
		0.32f;

	/**
	 * @brief 深場の透明度
	 */
	float DeepAlpha =
		0.70f;
};