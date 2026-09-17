#pragma once

/**
 * @file EnemyAIComponent.h
 * @brief Enemyの簡易AIを管理するComponent
 */

#include "Component.h"
#include "GridPosition.h"

class GameObject;

 /**
  * @brief Enemy AI Component
  *
  * @details
  * 現在はPlayerへ1マス接近する簡易AI。
  *
  * 将来的には、
  * ・索敵
  * ・攻撃
  * ・逃走
  * ・巡回
  * ・A*経路探索
  * などへ拡張する。
  */
class EnemyAIComponent :
	public Component
{
public:

	EnemyAIComponent() = default;
	~EnemyAIComponent() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw() override;

	/**
	 * @brief Enemyの1ターン分の行動
	 *
	 * @param playerPosition PlayerのGrid座標
	 *
	 * @return 行動を実行できた場合true
	 */
	bool Act(
		GameObject* player);

	void DrawDebugUI() override;

	const char* GetComponentName() const override
	{
		return "EnemyAIComponent";
	}

private:

	/**
	 * @brief Playerへ近づく移動を試みる
	 */
	bool TryMoveToward(
		const GridPosition& playerPosition);
};