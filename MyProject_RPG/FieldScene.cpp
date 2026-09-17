#include "FieldScene.h"
#include "DebugUI.h"
#include "Renderer.h"

// Component
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "AudioSourceComponent.h"
#include "GridPositionComponent.h"
#include "PlayerControllerComponent.h"
#include "EnemyAIComponent.h"
#include "CharacterStatusComponent.h"
#include "FieldExitComponent.h"

#include <ModelLoader.h>

#include "InputManager.h"
#include "SoundManager.h"
#include "QuestManager.h"
#include "SceneManager.h"

FieldScene::FieldScene()
{

}

void FieldScene::Initialize()
{
	if (!m_GridMap.Initialize(
		10,
		10,
		2.0f))
	{
		return;
	}
	// GameObjectの初期化
	{
		//====================
		// Player生成
		//====================
		{
			auto* warrior =
				m_gameObjectManager.Create<GameObject>();

			if (!warrior)
			{
				return;
			}

			warrior->SetName("Warrior");
			warrior->SetTag(Tag::Player);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					warrior->AddComponent<TransformComponent>();

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
						3.3f,
						0.0f);

					// モデルサイズに応じて調整する
					transform->SetScale(
						0.01f);
				}
			}
			//====================
			// ModelComponent
			//====================
			{
				auto* model =
					warrior->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Player/Warrior.fbx");
					// Material[0]へTextureを手動設定
					const bool texture0Result =
						model->SetTexture(
							0,
							"Assets/Models/Player/Warrior_Texture.png");
					// Material[1]へTextureを手動設定
					const bool texture1Result =
						model->SetTexture(
							1,
							"Assets/Models/Player/Warrior_Sword_Texture.png");

					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle, true, 0.0f);
				}
			}
			//====================
			// Audio
			//====================
			{
				auto* audioSource =
					warrior->AddComponent<
					AudioSourceComponent>();

				audioSource->SetSoundPath(
					L"Assets/Sound/BGM.wav");

				audioSource->Set3D(
					true);

				audioSource->SetLoop(
					true);

				audioSource->SetVolume(
					1.0f);

				audioSource->SetCategory(
					SoundCategory::SE);

				audioSource->SetDistanceScaler(
					20.0f);

				audioSource->Play();
			}
			//====================
			// GridPosition
			//====================
			auto* gridPosition =
				warrior->AddComponent<
				GridPositionComponent>();

			gridPosition->SetGridMap(
				&m_GridMap);

			const bool result =
				gridPosition->SetGridPosition(
					GridPosition
					{
						2,
						2
					});

			auto* controller =
				warrior->AddComponent<
				PlayerControllerComponent>();
			controller->SetTurnManager(
				&m_TurnManager);

			auto* status =
				warrior->AddComponent<
				CharacterStatusComponent>();

			status->SetMaxHP(10);
			status->SetAttackPower(3);
		}
		{
			auto* enemy =
				m_gameObjectManager.Create<GameObject>();

			if (!enemy)
			{
				return;
			}

			enemy->SetName("Enemy");
			enemy->SetTag(Tag::Enemy);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					enemy->AddComponent<TransformComponent>();

				if (transform)
				{
					// 仮カメラが原点を向いているため、
					// まずはモデルを原点に配置して確認する
					transform->SetPosition(
						2.0f,
						0.0f,
						0.0f);

					transform->SetRotation(
						0.0f,
						3.3f,
						0.0f);

					// モデルサイズに応じて調整する
					transform->SetScale(
						0.01f);
				}
			}
			//====================
			// ModelComponent
			//====================
			{
				auto* model =
					enemy->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Enemy/Alien.fbx");
					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle, true, 0.0f);
				}
			}
			//====================
			// GridPosition
			//====================
			{
				auto* gridPosition =
					enemy->AddComponent<
					GridPositionComponent>();
				gridPosition->SetGridMap(&m_GridMap);
				gridPosition->SetGridPosition(
					GridPosition
					{
						4,
						2
					});
			}
			enemy->AddComponent<
				EnemyAIComponent>();
			auto* status =
				enemy->AddComponent<
				CharacterStatusComponent>();

			status->SetMaxHP(5);
			status->SetAttackPower(2);
		}

		{
			auto* monk =
				m_gameObjectManager.Create<GameObject>();

			if (!monk)
			{
				return;
			}

			monk->SetName("Monk");
			monk->SetTag(Tag::NPC);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					monk->AddComponent<TransformComponent>();

				if (transform)
				{
					// 仮カメラが原点を向いているため、
					// まずはモデルを原点に配置して確認する
					transform->SetPosition(
						-2.0f,
						0.0f,
						0.0f);

					transform->SetRotation(
						0.0f,
						3.3f,
						0.0f);

					// モデルサイズに応じて調整する
					transform->SetScale(
						0.01f);
				}
			}
			//====================
			// ModelComponent
			//====================
			{
				auto* model =
					monk->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Player/Monk.fbx");
					// Material[0]へTextureを手動設定
					const bool texture0Result =
						model->SetTexture(
							0,
							"Assets/Models/Player/Monk_Texture.png");

					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle, true, 0.0f);
				}
			}
			//====================
			// GridPosition
			//====================
			auto* gridPosition =
				monk->AddComponent<
				GridPositionComponent>();
			gridPosition->SetGridMap(&m_GridMap);
			gridPosition->SetGridPosition(
				GridPosition
				{
					4,
					4
				});
		}
		{
			auto* rogue =
				m_gameObjectManager.Create<GameObject>();

			if (!rogue)
			{
				return;
			}

			rogue->SetName("Rogue");
			rogue->SetTag(Tag::NPC);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					rogue->AddComponent<TransformComponent>();

				if (transform)
				{
					// 仮カメラが原点を向いているため、
					// まずはモデルを原点に配置して確認する
					transform->SetPosition(
						4.0f,
						0.0f,
						0.0f);

					transform->SetRotation(
						0.0f,
						3.3f,
						0.0f);

					// モデルサイズに応じて調整する
					transform->SetScale(
						0.01f);
				}
			}
			//====================
			// ModelComponent
			//====================
			{
				auto* model =
					rogue->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Player/Rogue.fbx");
					// Material[0]へTextureを手動設定
					const bool texture0Result =
						model->SetTexture(
							0,
							"Assets/Models/Player/Rogue_Texture.png");
					// Material[1]へTextureを手動設定
					const bool texture1Result =
						model->SetTexture(
							1,
							"Assets/Models/Player/Rogue_Dagger_Texture.png");

					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle, true, 0.0f);
				}
			}
			//====================
			// GridPosition
			//====================
			auto* gridPosition =
				rogue->AddComponent<
				GridPositionComponent>();
			gridPosition->SetGridMap(&m_GridMap);
			gridPosition->SetGridPosition(
				GridPosition
				{
					7,
					7
				});
		}
		{
			auto* wizard =
				m_gameObjectManager.Create<GameObject>();

			if (!wizard)
			{
				return;
			}

			wizard->SetName("Wizard");
			wizard->SetTag(Tag::NPC);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					wizard->AddComponent<TransformComponent>();

				if (transform)
				{
					// 仮カメラが原点を向いているため、
					// まずはモデルを原点に配置して確認する
					transform->SetPosition(
						-4.0f,
						0.0f,
						0.0f);

					transform->SetRotation(
						0.0f,
						3.3f,
						0.0f);

					// モデルサイズに応じて調整する
					transform->SetScale(
						0.01f);
				}
			}
			//====================
			// ModelComponent
			//====================
			{
				auto* model =
					wizard->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Player/Wizard.fbx");
					// Material[0]へTextureを手動設定
					const bool texture0Result =
						model->SetTexture(
							0,
							"Assets/Models/Player/Wizard_Texture.png");
					// Material[1]へTextureを手動設定
					const bool texture1Result =
						model->SetTexture(
							1,
							"Assets/Models/Player/Wizard_Staff_Texture.png");

					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle, true, 0.0f);
				}
			}
			//====================
			// GridPosition
			//====================
			auto* gridPosition =
				wizard->AddComponent<
				GridPositionComponent>();
			gridPosition->SetGridMap(&m_GridMap);
			gridPosition->SetGridPosition(
				GridPosition
				{
					9,
					9
				});
		}

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
						10.0f,
						5.0f);

					// 原点方向を向く初期値
					cameraTransform->SetRotation(
						-1.0f,
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

		//====================
		// Directional Light
		//====================
		{
			DirectionalLight directionalLight;

			directionalLight.Direction =
				Vector3(
					0.5f,
					-1.0f,
					0.4f);

			directionalLight.LightColor =
				Color(
					1.0f,
					0.97f,
					0.90f,
					1.0f);

			directionalLight.Intensity =
				0.90f;

			Renderer::SetDirectionalLight(
				directionalLight);

			//=====================================================
			// Ambient Light
			//=====================================================

			Renderer::SetAmbientLight(
				Color(
					0.14f,
					0.17f,
					0.22f,
					1.0f));
		}
		//=====================================================
		// Toon Shading
		//=====================================================
		{
			ToonShading toon;

			toon.HighlightThreshold =
				0.68f;

			toon.ShadowThreshold =
				0.35f;

			// 日中なので中間色はかなり明るめ
			toon.MidToneIntensity =
				0.80f;

			// 影でも色が潰れない程度
			toon.ShadowIntensity =
				0.50f;

			Renderer::SetToonShading(
				toon);
		}
		{
			OutlineSetting outline;

			outline.Width =
				0.015f;

			outline.OutlineColor =
				Color(
					0.02f,
					0.02f,
					0.02f,
					1.0f);

			Renderer::SetOutlineSetting(
				outline);
		}
	}
	{
		auto* townExit =
			m_gameObjectManager.Create<GameObject>();

		townExit->SetName(
			"TownExit");

		townExit->SetTag(
			Tag::None);

		//====================
		// Transform
		//====================

		auto* transform =
			townExit->AddComponent<
			TransformComponent>();
		transform->SetRotation(Vector3(-1.3f,0.0f,0.0f));

		//====================
		// ModelComponent
		//====================
		{
			auto* model =
				townExit->AddComponent<ModelComponent>();

			if (model)
			{
				// 使用するモデルの実際のパスに変更する
				const bool result =
					model->SetModel(
						"Assets/Models/field/House_1.fbx");
			}
		}

		//====================
		// Exit
		//====================
		auto* exit =
			townExit->AddComponent<
			FieldExitComponent>();

		exit->SetGridPosition(
			GridPosition
			{
				0,
				0
			});

		exit->SetTargetScene(
			"TownScene");
	}
	std::vector<GameObject*> m_Player =
		m_gameObjectManager.FindByTag(Tag::Player);
	auto* ctr = m_Player[0]->GetComponent<PlayerControllerComponent>();
	std::vector<GameObject*> m_Enemy =
		m_gameObjectManager.FindByTag(Tag::Enemy);
	ctr->SetAttackTarget(m_Enemy[0]);

	// クエスト初期化
	m_CurrentQuest.Setup(
		"Enemy Hunt",
		QuestType::KillEnemy,
		1);
	m_TurnManager.Initialize();
	m_gameObjectManager.Initialize();

}

void FieldScene::Finalize()
{
	// GameObjectの終了処理
	m_gameObjectManager.Clear();
}

void FieldScene::Update(
	uint64_t delta)
{
	//=====================================================
	// Enemy Turn
	//=====================================================

	if (m_TurnManager.IsEnemyTurn())
	{
		//=================================================
		// Player取得
		//=================================================

		std::vector<GameObject*> players =
			m_gameObjectManager.FindByTag(
				Tag::Player);

		//=================================================
		// Enemy取得
		//=================================================

		std::vector<GameObject*> enemies =
			m_gameObjectManager.FindByTag(
				Tag::Enemy);

		//=================================================
		// Enemy全員の行動
		//=================================================

		if (!players.empty())
		{
			GameObject* player =
				players[0];

			for (GameObject* enemy :
				enemies)
			{
				if (!enemy)
				{
					continue;
				}

				//=========================================
				// 死亡確認
				//=========================================

				auto* enemyStatus =
					enemy->GetComponent<
					CharacterStatusComponent>();

				if (!enemyStatus)
				{
					continue;
				}

				// 死亡済みEnemyは行動させない
				if (enemyStatus->IsDead())
				{
					continue;
				}

				//=========================================
				// Enemy AI
				//=========================================

				auto* enemyAI =
					enemy->GetComponent<
					EnemyAIComponent>();

				if (enemyAI)
				{
					enemyAI->Act(
						player);
				}
			}
		}

		//=================================================
		// Enemy全員の行動終了
		//=================================================

		m_TurnManager.EndEnemyTurn();
	}

	//=====================================================
	// GameObject Update
	//=====================================================

	m_gameObjectManager.Update(
		delta);

	//=================
	// 帰還判定
	//=================
	std::vector<GameObject*> players =
		m_gameObjectManager.FindByTag(
			Tag::Player);

	GameObject* exitObject =
		m_gameObjectManager.FindByName(
			"TownExit");

	if (!players.empty() &&
		exitObject)
	{
		GameObject* player =
			players[0];

		auto* playerGrid =
			player->GetComponent<
			GridPositionComponent>();

		auto* fieldExit =
			exitObject->GetComponent<
			FieldExitComponent>();

		if (playerGrid &&
			fieldExit)
		{
			const GridPosition playerPosition =
				playerGrid->GetGridPosition();

			const GridPosition exitPosition =
				fieldExit->GetGridPosition();

			if (playerPosition ==
				exitPosition)
			{
				if (INPUT_MANAGER.IsKeyPressed(
					KeyCode::E))
				{
					SceneManager::SetCurrentScene(
						fieldExit->
						GetTargetScene());
				}
			}
		}
	}

	//=====================================================
	// Enemy死亡処理
	//=====================================================

	std::vector<GameObject*> enemies =
		m_gameObjectManager.FindByTag(
			Tag::Enemy);

	for (GameObject* enemy :
		enemies)
	{
		if (!enemy)
		{
			continue;
		}

		auto* enemyStatus =
			enemy->GetComponent<
			CharacterStatusComponent>();

		if (!enemyStatus ||
			!enemyStatus->IsDead())
		{
			continue;
		}

		OutputDebugStringA(
			"[FieldScene] Enemy defeated.\n");

		//=============================================
		// Questへ討伐通知
		//=============================================

		QUEST_MANAGER.
			NotifyEnemyKilled();

		//=============================================
		// Playerが死亡Enemyをターゲットしている場合への
		// 仮対応
		//=============================================

		std::vector<GameObject*> players =
			m_gameObjectManager.FindByTag(
				Tag::Player);

		for (GameObject* player :
			players)
		{
			if (!player)
			{
				continue;
			}

			auto* controller =
				player->GetComponent<
				PlayerControllerComponent>();

			if (controller)
			{
				// 現在は単一ターゲット方式なので解除。
				// Mouse選択方式へ移行した段階で変更予定。
				controller->SetAttackTarget(
					nullptr);
			}
		}

		//=============================================
		// 削除予約
		//=============================================

		enemy->Destroy();
	}
}

void FieldScene::Draw(uint64_t delta)
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
			// GameObjectのDebugUI描画
			m_gameObjectManager.DrawDebugUI();

			// Quest関係のDebugUI描画
			ImGui::Begin("Quest");
			ImGui::SeparatorText(
				"Quests");
			for (const Quest& quest :
				QUEST_MANAGER.GetQuests())
			{
				ImGui::Text(
					"%s",
					quest.GetName().c_str());

				ImGui::Text(
					"Progress: %d / %d",
					quest.GetCurrentCount(),
					quest.GetRequiredCount());

				const char* stateText =
					"Unknown";

				switch (quest.GetState())
				{
				case QuestState::Inactive:
					stateText = "Inactive";
					break;

				case QuestState::Active:
					stateText = "Active";
					break;

				case QuestState::ReadyToReport:
					stateText = "Ready To Report";
					break;

				case QuestState::Completed:
					stateText = "Completed";
					break;
				}

				ImGui::Text(
					"State: %s",
					stateText);

				ImGui::Separator();
			}
			ImGui::End();
		});

}