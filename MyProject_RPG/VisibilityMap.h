#pragma once

#include <vector>

#include "GridPosition.h"
#include "TerrainMapData.h"
#include "VisibilityState.h"

/**
 * @brief Fieldの探索状態を管理するMap
 *
 * @details
 * Terrainそのものの情報ではなく、
 * 「Playerがその場所を知っているか」を管理する。
 *
 * GridMap      : Gameplay情報
 * TerrainMap   : 地形情報
 * VisibilityMap: Playerの認識情報
 *
 * と責務を分離する。
 */
class VisibilityMap
{
public:

	VisibilityMap() = default;
	~VisibilityMap() = default;

	//====================
	// Lifecycle
	//====================

	/**
	 * @brief TerrainDataからMapサイズを決定して初期化
	 */
	bool Initialize(
		const TerrainMapData& terrainData);

	/**
	 * @brief 全データ削除
	 */
	void Clear();

	//====================
	// Visibility Update
	//====================

	/**
	 * @brief 現在VisibleなCellをExploredへ戻す
	 *
	 * @details
	 * 新しい視界を計算する直前に呼ぶ。
	 *
	 * Unexploredへ戻すことはない。
	 */
	void ClearCurrentVisibility();

	/**
	 * @brief 指定Cellを現在視界内にする
	 */
	void SetVisible(
		const GridPosition& position);

	/**
	 * @brief 円形範囲をVisibleにする
	 *
	 * @param center 中心Grid
	 * @param radius 視界半径
	 */
	void RevealCircle(
		const GridPosition& center,
		int radius);

	//====================
	// Query
	//====================

	/**
	 * @brief 指定Cellの探索状態取得
	 */
	VisibilityState GetState(
		const GridPosition& position) const;

	bool IsInside(
		const GridPosition& position) const;

	bool IsVisible(
		const GridPosition& position) const;

	bool IsExplored(
		const GridPosition& position) const;

	/**
	 * @brief 一度でも探索済みか
	 *
	 * Visibleも探索済みとして扱う。
	 */
	bool HasBeenExplored(
		const GridPosition& position) const;

	//====================
	// Getter
	//====================

	int GetWidth() const
	{
		return m_Width;
	}

	int GetHeight() const
	{
		return m_Height;
	}

	/**
	 * @brief 全Visibility状態取得
	 *
	 * GPU Texture更新やSaveData生成に使用する。
	 */
	const std::vector<VisibilityState>&
		GetStates() const
	{
		return m_States;
	}

private:

	/**
	 * @brief GridPosition → 配列Index
	 */
	size_t ToIndex(
		const GridPosition& position) const;

private:

	int m_Width =
		0;

	int m_Height =
		0;

	/**
	 * @brief Gridごとの探索状態
	 */
	std::vector<VisibilityState>
		m_States;
};