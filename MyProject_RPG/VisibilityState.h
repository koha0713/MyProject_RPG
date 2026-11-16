#pragma once

/**
 * @brief Playerから見たGridの探索状態
 */
enum class VisibilityState
{
	/**
	 * @brief 一度も探索していない
	 *
	 * 羊皮紙Textureで表示する。
	 */
	Unexplored = 0,

	/**
	 * @brief 一度探索したが現在は視界外
	 *
	 * 地形・固定Objectのみ表示する。
	 */
	Explored,

	/**
	 * @brief 現在Playerの視界内
	 *
	 * 全て通常表示する。
	 */
	Visible
};