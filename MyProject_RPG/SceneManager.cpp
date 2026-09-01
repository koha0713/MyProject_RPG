#include "SceneManager.h"
#include "SceneClassFactory.h"

//==============================
// ライフサイクル
//==============================
void SceneManager::Initialize()
{

}

/**
 * @brief 終了処理：登録されているシーンを全て破棄
 */
void SceneManager::Finalize()
{
	// 登録されている全てのシーンの終了処理
	for (auto& s : m_scenes)
	{
		s.second->Finalize();
	}

	m_scenes.clear();
	m_currentSceneName.clear();

}

void SceneManager::Update(uint64_t delta)
{
	// 現在のシーンを更新
	m_scenes[m_currentSceneName]->Update(delta);

}

void SceneManager::Draw(uint64_t delta)
{
	// 現在のシーンを描画
	m_scenes[m_currentSceneName]->Draw(delta);

}

void SceneManager::SetCurrentScene(std::string currentscenename)
{
	m_currentSceneName = currentscenename;
	auto obj = SceneClassFactory::GetInstance().Create(currentscenename);
	obj->Initialize();
	m_scenes[m_currentSceneName] = std::move(obj);
}