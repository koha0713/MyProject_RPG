#include "TestScene.h"

TestScene::TestScene()
{

}

void TestScene::Initialize()
{
	// GameObject‚Ì‰Šú‰»
	{
		auto* player = m_gameObjectManager.Create<GameObject>();

		player->SetName("Player");
		player->SetTag(Tag::Player);

		// ‚±‚±‚ÅComponent‚ğ’Ç‰Á‚·‚é‚±‚Æ‚ª‚Å‚«‚Ü‚·
		// player->AddComponent<TransformComponent>();
	}


}

void TestScene::Finalize()
{
	// GameObject‚ÌI—¹ˆ—
	m_gameObjectManager.Clear();
}

void TestScene::Update(uint64_t delta)
{
	// GameObject‚ÌXV
	m_gameObjectManager.Update();
}

void TestScene::Draw(uint64_t delta)
{
	// GameObject‚Ì•`‰æ
	m_gameObjectManager.Draw();
}