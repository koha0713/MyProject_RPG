#include "TownScene.h"
#include "DebugUI.h"
#include "Renderer.h"

// Component
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "AudioSourceComponent.h"
#include <ModelLoader.h>

#include "InputManager.h"
#include "SoundManager.h"
#include "QuestManager.h"
#include "SceneManager.h"

TownScene::TownScene()
{

}

void TownScene::Initialize()
{
	// GameObjectの初期化
	{
		//====================
		// Camera生成
		//====================
		{
			auto* cameraObject =
				m_gameObjectManager.Create<GameObject>();

			if (cameraObject)
			{
				cameraObject->SetName(
					"MainCamera");

				//====================
				// Transform
				//====================

				auto* cameraTransform =
					cameraObject->
					AddComponent<TransformComponent>();

				if (cameraTransform)
				{
					cameraTransform->SetPosition(
						0.0f,
						2.0f,
						5.0f);

					// 原点方向を向く初期値
					cameraTransform->SetRotation(
						0.0f,
						0.0f,
						0.0f);
				}

				//====================
				// Camera
				//====================

				m_MainCamera =
					cameraObject->
					AddComponent<CameraComponent>();

				if (m_MainCamera)
				{
					m_MainCamera->
						SetFieldOfView(
							60.0f);

					m_MainCamera->
						SetNearClip(
							0.1f);

					m_MainCamera->
						SetFarClip(
							1000.0f);
				}
			}
		}
	}

	m_gameObjectManager.Initialize();

}

void TownScene::Finalize()
{
	// GameObjectの終了処理
	m_gameObjectManager.Clear();
}

void TownScene::Update(uint64_t delta)
{
	//====================
	// Keyboard Test
	//====================

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::Space))
	{
		SOUND_MANAGER.Play2D(
			L"Assets/Sound/Test.wav");
		OutputDebugStringA(
			"[Input] Space Pressed\n");
	}

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::W))
	{
		m_BGMHandle =
			SOUND_MANAGER.Play3D(
				L"Assets/Sound/BGM.wav",
				Vector3(
					0.0f,
					0.0f,
					0.0f),
				true,
				1.0f,
				SoundCategory::BGM);
		if (!m_BGMHandle.IsValid())
		{
			OutputDebugStringA(
				"[TownScene] Play3D Failed\n");
		}
		OutputDebugStringA(
			"[Input] W Down\n");
	}

	if (INPUT_MANAGER.IsKeyPressed(
		KeyCode::S))
	{
		SOUND_MANAGER.Stop(
			m_BGMHandle);

		m_BGMHandle = {};
		OutputDebugStringA(
			"[Input] W Released\n");
	}

	//====================
	// Mouse Test
	//====================

	if (INPUT_MANAGER.IsMousePressed(
		MouseButton::Left))
	{
		OutputDebugStringA(
			"[Input] Left Mouse Pressed\n");
	}

	const Vector2& mousePosition =
		INPUT_MANAGER.GetMousePosition();

	const Vector2& mouseDelta =
		INPUT_MANAGER.GetMouseDelta();

	const float wheel =
		INPUT_MANAGER.GetMouseWheel();

	// GameObjectの更新
	m_gameObjectManager.Update(delta);

	
}

void TownScene::Draw(uint64_t delta)
{
	//====================
	// Camera設定
	//====================

	if (m_MainCamera)
	{
		Renderer::SetCamera(
			m_MainCamera->
			GetViewMatrix(),

			m_MainCamera->
			GetProjectionMatrix());
	}

	// GameObjectの描画
	m_gameObjectManager.Draw();

	// デバッグUIの描画
	DebugUI::RegisterDebugFunction([this]()
		{
			m_gameObjectManager.DrawDebugUI();
			DrawQuestReportUI();
		});

}

void TownScene::DrawQuestReportUI()
{
	ImGui::SeparatorText("Guild");

	std::vector<Quest*> reportableQuests =
		QUEST_MANAGER.GetReportableQuests();

	if (ImGui::Button("Go To Field"))
	{
		SceneManager::SetCurrentScene(
			"FieldScene");
	}

	if (reportableQuests.empty())
	{
		ImGui::Text("No quests ready to report.");
		return;
	}

	for (Quest* quest :
		reportableQuests)
	{
		if (!quest)
		{
			continue;
		}

		ImGui::Text(
			"%s",
			quest->GetName().c_str());

		ImGui::Text(
			"Progress: %d / %d",
			quest->GetCurrentCount(),
			quest->GetRequiredCount());

		std::string buttonLabel =
			"Report##" +
			quest->GetName();

		if (ImGui::Button(
			buttonLabel.c_str()))
		{
			if (quest->Report())
			{
				OutputDebugStringA(
					"[Guild] Quest Reported.\n");
			}
		}

		ImGui::Separator();
	}
}