#include "TestScene.h"
#include "DebugUI.h"

// Component
#include "ModelComponent.h"
#include "TransformComponent.h"

TestScene::TestScene()
{

}

void TestScene::Initialize()
{
	// GameObjectの初期化
	{
		//====================
		// Player生成
		//====================

		auto* player =
			m_gameObjectManager.Create<GameObject>();

		if (!player)
		{
			return;
		}

		player->SetName("Player");
		player->SetTag(Tag::Player);

		//====================
		// TransformComponent
		//====================

		auto* transform =
			player->AddComponent<TransformComponent>();

		if (transform)
		{
			// 仮カメラが原点を向いているため、
			// まずはモデルを原点に配置して確認する
			transform->SetPosition(
				0.0f,
				0.0f,
				0.0f);

			transform->SetRotation(
				0.0f,
				0.0f,
				0.0f);

			// モデルサイズに応じて調整する
			transform->SetScale(
				1.0f);
		}

		//====================
		// ModelComponent
		//====================

		auto* model =
			player->AddComponent<ModelComponent>();

		if (model)
		{
			// 使用するモデルの実際のパスに変更する
			const bool result =
				model->SetModel(
					"Assets/Models/Warrior_Run.fbx");

			if (!result)
			{
				OutputDebugStringA(
					"[TestScene] Model load failed.\n");
			}
			else
			{
				OutputDebugStringA(
					"[TestScene] Model load succeeded.\n");
			}
		}


		// ここでComponentを追加することができます
		// player->AddComponent<TransformComponent>();
	}

	m_gameObjectManager.Initialize();

}

void TestScene::Finalize()
{
	// GameObjectの終了処理
	m_gameObjectManager.Clear();
}

void TestScene::Update(uint64_t delta)
{
	// GameObjectの更新
	m_gameObjectManager.Update();
}

void TestScene::Draw(uint64_t delta)
{
	// GameObjectの描画
	m_gameObjectManager.Draw();

	// デバッグUIの描画
	DebugUI::RegisterDebugFunction([this]()
		{
			ImGui::Begin("Test");
			ImGui::Text("あああ");
			ImGui::End();
		});

}