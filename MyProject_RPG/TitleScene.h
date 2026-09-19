#pragma once

#include "IScene.h"
#include "SceneClassFactory.h"
#include "GameObjectManager.h"

class GameObject;

/**
 * @brief Title Scene
 */
class TitleScene :
	public IScene
{
public:
	//====================
	// ライフサイクル
	//====================
	void Initialize() override;
	void Finalize() override;
	void Update(uint64_t delta) override;
	void Draw(uint64_t delta) override;

private:

	GameObjectManager
		m_GameObjectManager;

	//====================
	// UI Objects
	//====================
	GameObject* m_Background = nullptr;
	GameObject* m_StartButton = nullptr;
	GameObject* m_ExitButton = nullptr;

private:
	//====================
	// 補助関数
	//====================
	void UpdateButtonVisual(GameObject* buttonObject);

	void CreateBackground();
	void CreateStartButton();
	void CreateExitButton();
};

// Scene登録
REGISTER_CLASS(TitleScene)