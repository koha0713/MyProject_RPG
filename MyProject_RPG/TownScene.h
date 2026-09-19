#pragma once

/**
 * @file TownScene.h
 * @brief 町画面を管理するScene
 */

#include <cstdint>

#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"

class GameObject;

/**
 * @brief Town内の現在地
 *
 * @details
 * Town内部の施設はSceneを分けず、
 * TownScene内の状態として管理する。
 *
 * 将来的にShop / Innなどを追加しやすくする。
 */
enum class TownLocation
{
	Overview,
	Guild
};

/**
 * @brief Town Scene
 *
 * @details
 * 2D背景TextureとUIButtonのみで操作する。
 */
class TownScene :
	public IScene
{
public:

	TownScene() = default;
	~TownScene() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw(
		uint64_t delta) override;

private:

	//====================
	// UI生成
	//====================

	/**
	 * @brief 背景UI生成
	 */
	void CreateBackground();

	/**
	 * @brief 3つの共通Buttonを生成
	 */
	void CreateButtons();

	/**
	 * @brief 現在地に合わせてUI表示を更新
	 */
	void RefreshLocationUI();

	/**
	 * @brief Buttonの見た目更新
	 */
	void UpdateButtonVisual(
		GameObject* buttonObject);

	/**
	 * @brief Buttonの矩形とTextureを設定する
	 */
	void ConfigureButton(
		GameObject* buttonObject,
		const char* texturePath,
		float x,
		float y,
		float width,
		float height);

	/**
	 * @brief Buttonを非表示にする
	 *
	 * @details
	 * 現在はUIRectを0サイズにすることで
	 * 描画とHit判定の両方を無効化する。
	 */
	void HideButton(
		GameObject* buttonObject);

	//====================
	// Location
	//====================

	/**
	 * @brief Town内の場所を変更
	 */
	void ChangeLocation(
		TownLocation location);

	//====================
	// Input
	//====================

	void UpdateTownOverview();

	void UpdateGuild();

	//====================
	// Guild
	//====================

	void AcceptQuest();

	void ReportQuest();

	/**
	 * @brief GuildのQuest一覧を一時的にImGui表示する
	 *
	 * @note β版ではQuest情報確認用として使用。
	 *       将来的に独自UIへ置換する。
	 */
	void DrawGuildQuestListUI();

private:

	GameObjectManager
		m_GameObjectManager;

	TownLocation
		m_CurrentLocation =
		TownLocation::Overview;

	//====================
	// UI Objects
	//====================

	GameObject*
		m_Background =
		nullptr;

	/**
	 * @brief 状態によって役割を変えるButton
	 *
	 * Overview:
	 * Primary   = Guild
	 * Secondary = TownExit
	 *
	 * Guild:
	 * Primary   = Accept
	 * Secondary = Report
	 */
	GameObject*
		m_PrimaryButton =
		nullptr;

	GameObject*
		m_SecondaryButton =
		nullptr;

	GameObject*
		m_BackButton =
		nullptr;
};

REGISTER_CLASS(TownScene)