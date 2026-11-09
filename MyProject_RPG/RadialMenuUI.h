#pragma once

/**
 * @file RadialMenuUI.h
 * @brief Player Action選択用Radial Menu
 */

#include "PlayerActionType.h"

class GameObject;
class GameObjectManager;

/**
 * @brief Radial Action Menu
 *
 * @details
 * PlayerActionType 6種類を円形に配置する。
 *
 * このClassは表示のみ担当する。
 *
 * Mouse入力・Drag方向判定・Action決定は
 * FieldScene側が担当する。
 */
class RadialMenuUI
{
public:

	RadialMenuUI() = default;
	~RadialMenuUI() = default;

	//=================================================
	// Lifecycle
	//=================================================

	void Initialize(
		GameObjectManager& manager);

	void Finalize();

	//=================================================
	// Visibility
	//=================================================

	void SetVisible(
		bool visible);

	bool IsVisible() const
	{
		return m_Visible;
	}

	//=================================================
	// Position
	//=================================================

	/**
	 * @brief Menu中心位置をScreen座標で設定
	 */
	void SetCenter(
		float x,
		float y);

	//=================================================
	// Highlight
	//=================================================

	/**
	 * @brief 選択候補Actionを設定
	 */
	void SetHighlightedAction(
		PlayerActionType type);

	/**
	 * @brief Highlight解除
	 *
	 * @details
	 * Dead Zone内の場合などに使用する。
	 */
	void ClearHighlight();

private:

	//=================================================
	// Create
	//=================================================

	void CreateActionTexts();

	GameObject* CreateText(
		const char* name,
		const wchar_t* text);

	//=================================================
	// Visual
	//=================================================

	void UpdateLayout();

	void UpdateHighlightVisual();

	void HideObject(
		GameObject* object);

	/**
	 * @brief ActionTypeから表示Objectを取得
	 */
	GameObject* GetActionObject(
		PlayerActionType type) const;

private:

	GameObjectManager*
		m_Manager =
		nullptr;

	bool
		m_Visible =
		false;

	float
		m_CenterX =
		640.0f;

	float
		m_CenterY =
		360.0f;

	/**
	 * @brief Menu半径
	 */
	float
		m_Radius =
		115.0f;

	/**
	 * @brief Highlight中か
	 */
	bool
		m_HasHighlight =
		false;

	PlayerActionType
		m_HighlightedAction =
		PlayerActionType::Move;

	//=================================================
	// Action Text
	//=================================================

	GameObject*
		m_MoveText =
		nullptr;

	GameObject*
		m_NormalAttackText =
		nullptr;

	GameObject*
		m_Skill1Text =
		nullptr;

	GameObject*
		m_Skill2Text =
		nullptr;

	GameObject*
		m_UltimateText =
		nullptr;

	GameObject*
		m_ItemText =
		nullptr;

	/**
	 * @brief Dead Zone表示
	 *
	 * 最初は文字だけ表示。
	 */
	GameObject*
		m_CenterText =
		nullptr;
};