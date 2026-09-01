#include "TestScene.h"
#include "DebugUI.h"

TestScene::TestScene()
{

}

void TestScene::Initialize()
{
	// GameObjectの初期化
	{
		auto* player = m_gameObjectManager.Create<GameObject>();

		player->SetName("Player");
		player->SetTag(Tag::Player);

		// ここでComponentを追加することができます
		// player->AddComponent<TransformComponent>();
	}


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