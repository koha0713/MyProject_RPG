#pragma once
#include <string>
#include <memory>
#include <cstdint>
#include <array>
#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"

class CameraComponent;

class TestScene : public IScene
{
public:
	//====================
	// ライフサイクル
	//====================
	explicit TestScene();
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
	CameraComponent* m_MainCamera = nullptr;
};
// Scene登録
REGISTER_CLASS(TestScene)