#pragma once

/**
 * @file GridPositionComponent.h
 * @brief GameObjectのGrid座標を管理するComponent
 */

#include "Component.h"
#include "GridPosition.h"

class GridMap;

/**
 * @brief Grid Position Component
 *
 * @details
 * GameObjectがGrid上のどのCellに存在しているかを管理する。
 *
 * Grid座標変更時にTransformComponentへ
 * World座標を反映する。
 */
class GridPositionComponent :
	public Component
{
public:

	GridPositionComponent() = default;
	~GridPositionComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	//====================
	// Grid
	//====================

	/**
	 * @brief 使用するGridMapを設定
	 *
	 * @note
	 * GridMapの所有権は持たない。
	 */
	void SetGridMap(
		GridMap* gridMap);

	GridMap* GetGridMap() const
	{
		return m_GridMap;
	}

	/**
	 * @brief Grid座標を設定
	 *
	 * @return 配置成功時true
	 */
	bool SetGridPosition(
		const GridPosition& position);

	const GridPosition&
		GetGridPosition() const
	{
		return m_GridPosition;
	}

	/**
	 * @brief 指定方向へ移動を試みる
	 */
	bool TryMove(
		const GridPosition& direction);

	//====================
	// Debug
	//====================

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "GridPositionComponent";
	}

private:

	/**
	 * @brief Grid位置からTransform位置を更新
	 */
	void SyncTransform();

private:

	GridMap* m_GridMap =
		nullptr;

	GridPosition m_GridPosition;

	bool m_IsRegistered =
		false;
};