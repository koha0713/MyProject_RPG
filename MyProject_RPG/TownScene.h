#pragma once
#include <string>
#include <memory>
#include <cstdint>
#include <array>
#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"
#include "SoundHandle.h"

class CameraComponent;

class TownScene : public IScene
{
public:
	//====================
	// ライフサイクル
	//====================
	explicit TownScene();
	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw(uint64_t delta) override;

	//====================
	// デバッグ用関数(予定)
	//====================
	void DrawQuestReportUI();

	//====================
	// メンバ変数
	//====================
	GameObjectManager m_gameObjectManager;
	CameraComponent* m_MainCamera = nullptr;
	SoundHandle m_BGMHandle;
};
// Scene登録
REGISTER_CLASS(TownScene)