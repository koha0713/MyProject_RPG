#include "GameObjectManager.h"

//====================
// ライフサイクル
//====================

void GameObjectManager::Initialize()
{
	for (auto& object : m_GameObjects)
	{
		object->Initialize();
	}
}

void GameObjectManager::Finalize()
{
	for (auto& object : m_GameObjects)
	{
		object->Finalize();
	}
}

void GameObjectManager::Update()
{
	for (auto& object : m_GameObjects)
	{
		object->Update();
	}

	RemoveDestroyedObjects();
}

void GameObjectManager::Draw()
{
	for (auto& object : m_GameObjects)
	{
		object->Draw();
	}
}

void GameObjectManager::Clear()
{
	for(auto& object : m_GameObjects)
	{
		object->Finalize();
	}

	m_GameObjects.clear();
}

//====================
// GameObjectの検索
//====================

std::vector<GameObject*> GameObjectManager::FindByTag(Tag tag) const
{
	std::vector<GameObject*> result;
	for (auto& object : m_GameObjects)
	{
		if (object->GetTag() == tag)
		{
			result.push_back(object.get());
		}
	}
	return result;
}

GameObject* GameObjectManager::FindByName(const std::string& name) const
{
	for (auto& object : m_GameObjects)
	{
		if (object->GetName() == name)
		{
			return object.get();
		}
	}
	return nullptr;
}

//====================
// GameObjectの破棄
//====================

void GameObjectManager::RemoveDestroyedObjects()
{
	auto it = std::remove_if(
		m_GameObjects.begin(),
		m_GameObjects.end(),
		[](const std::unique_ptr<GameObject>& object)
		{
			if (object->IsDestroy())
			{
				object->Finalize();
				return true;
			}

			return false;
		});

	m_GameObjects.erase(it, m_GameObjects.end());
}