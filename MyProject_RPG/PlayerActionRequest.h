#pragma once

/**
 * @file PlayerActionRequest.h
 * @brief Player Action実行要求
 */

#include "PlayerActionType.h"
#include "GridPosition.h"
#include "GridPattern.h"

class GameObject;

/**
 * @brief Player Action Request
 *
 * @details
 * UIやFieldSceneからPlayerControllerへ
 * Action実行情報を渡すためのデータ。
 *
 * TargetModeによって使用する値が異なる。
 */
struct PlayerActionRequest
{
	/**
	 * @brief 実行するAction
	 */
	PlayerActionType Type =
		PlayerActionType::Move;

	/**
	 * @brief Unit Target
	 *
	 * TargetingMode::Unitで使用。
	 */
	GameObject* TargetObject =
		nullptr;

	/**
	 * @brief Grid Target
	 *
	 * TargetingMode::Gridで使用。
	 */
	GridPosition TargetGrid
	{
		0,
		0
	};

	/**
	 * @brief Direction Target
	 *
	 * TargetingMode::Directionで使用。
	 */
	GridDirection Direction =
		GridDirection::North;
};