#pragma once
#include <string>
#include <memory>
#include <cstdint>
#include <array>
#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"
#include "SoundHandle.h"
#include "GridMap.h"
#include "TurnManager.h"
#include "Quest.h"


class CameraComponent;

class FieldScene : public IScene
{
public:
	//====================
	// ライフサイクル
	//====================
	explicit FieldScene();
	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw(uint64_t delta) override;

	//====================
	// デバッグ用関数(予定)
	//====================


	//====================
	// メンバ変数
	//====================
	GameObjectManager m_gameObjectManager;
	TurnManager m_TurnManager;
	CameraComponent* m_MainCamera = nullptr;
	SoundHandle m_BGMHandle;
	GridMap m_GridMap;
	Quest m_CurrentQuest;
};
// Scene登録
REGISTER_CLASS(FieldScene)