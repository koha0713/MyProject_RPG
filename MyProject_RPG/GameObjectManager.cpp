#include "GameObjectManager.h"
#include "DebugUI.h"

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

void GameObjectManager::DrawDebugUI()
{
	if (ImGui::Begin("GameObject Manager"))
	{
		//====================
		// GameObjectなし
		//====================

		if (m_GameObjects.empty())
		{
			ImGui::Text(
				"No GameObjects.");

			ImGui::End();
			return;
		}

		// 削除などによってIndexが
	// 範囲外になっていた場合の保護
		if (m_SelectedGameObjectIndex >=
			m_GameObjects.size())
		{
			m_SelectedGameObjectIndex = 0;
			m_SelectedComponentIndex = 0;
		}

		GameObject* selectedGameObject =
			m_GameObjects[
				m_SelectedGameObjectIndex]
			.get();

		if (!selectedGameObject)
		{
			ImGui::End();
			return;
		}

		//=================================================
		// GameObject選択
		//=================================================

		const char* currentGameObjectName =
			selectedGameObject->
			GetName().
			c_str();

		if (ImGui::BeginCombo(
			"GameObject",
			currentGameObjectName))
		{
			for (size_t i = 0;
				i < m_GameObjects.size();
				++i)
			{
				GameObject* gameObject =
					m_GameObjects[i].get();

				if (!gameObject)
				{
					continue;
				}

				const bool selected =
					(i ==
						m_SelectedGameObjectIndex);

				if (ImGui::Selectable(
					gameObject->
					GetName().
					c_str(),
					selected))
				{
					m_SelectedGameObjectIndex =
						i;

					// GameObjectを変更したら
					// Component選択を先頭へ戻す
					m_SelectedComponentIndex =
						0;
				}

				if (selected)
				{
					ImGui::
						SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		// GameObjectを選び直した可能性があるため再取得
		selectedGameObject =
			m_GameObjects[
				m_SelectedGameObjectIndex]
			.get();

		if (!selectedGameObject)
		{
			ImGui::End();
			return;
		}

		ImGui::Separator();

		//=================================================
		// Component
		//=================================================

		const auto& components =
			selectedGameObject->
			GetComponents();

		if (components.empty())
		{
			ImGui::Text(
				"No Components.");

			ImGui::End();
			return;
		}

		if (m_SelectedComponentIndex >=
			components.size())
		{
			m_SelectedComponentIndex = 0;
		}

		Component* selectedComponent =
			components[
				m_SelectedComponentIndex]
			.get();

		if (!selectedComponent)
		{
			ImGui::End();
			return;
		}

		//=================================================
		// Component選択
		//=================================================

		if (ImGui::BeginCombo(
			"Component",
			selectedComponent->
			GetComponentName()))
		{
			for (size_t i = 0;
				i < components.size();
				++i)
			{
				Component* component =
					components[i].get();

				if (!component)
				{
					continue;
				}

				const bool selected =
					(i ==
						m_SelectedComponentIndex);

				if (ImGui::Selectable(
					component->
					GetComponentName(),
					selected))
				{
					m_SelectedComponentIndex =
						i;
				}

				if (selected)
				{
					ImGui::
						SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		//=================================================
		// Component固有DebugUI
		//=================================================

		selectedComponent =
			components[
				m_SelectedComponentIndex]
			.get();

		if (selectedComponent)
		{
			selectedComponent->
				DrawDebugUI();
		}
	}
	ImGui::End();
}