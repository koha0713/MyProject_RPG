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
#include "CharacterAnimationComponent.h"
#include "TerrainTileComponent.h"

#include <ModelLoader.h>
#include "MousePicker.h"
#include "Window.h"

#include "InputManager.h"
#include "SoundManager.h"
#include "QuestManager.h"
#include "SceneManager.h"
#include "GridRenderer.h"
#include "GridRangeRenderer.h"
#include "GridRangeCalculator.h"

#include "CharaMapData.h"
#include "CharaMapLoader.h"

#include "TerrainMapLoader.h"

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

	//=====================================================
	// TerrainMap
	//=====================================================

	if (!TerrainMapLoader::Load(
		"Assets/Map/TerrainMap.txt",
		m_TerrainMapData))
	{
		OutputDebugStringA(
			"[FieldScene] "
			"TerrainMap Load Failed.\n");
	}
	else
	{
		for (const TerrainCellData& cell :
			m_TerrainMapData.Cells)
		{
			CreateTerrainTile(
				cell);
		}
	}
	

	// GameObjectの初期化
	{
		//=====================================================
		// Character Map Load
		//=====================================================
		CharaMapData
			charaMapData;

		if (!CharaMapLoader::Load(
			"Assets/Map/CharaMap.txt",
			charaMapData))
		{
			OutputDebugStringA(
				"[FieldScene] "
				"Failed to load CharaMap.\n");

			return;
		}

		//=====================================================
		// Character生成
		//=====================================================

		for (const CharacterSpawnData& spawnData :
			charaMapData.Characters)
		{
			switch (spawnData.Kind)
			{
			case CharacterSpawnKind::Player:

				if (!CreatePlayer(
					spawnData))
				{
					OutputDebugStringA(
						"[FieldScene] "
						"Failed to create Player.\n");
				}

				break;

			case CharacterSpawnKind::Enemy:

				if (!CreateEnemy(
					spawnData))
				{
					OutputDebugStringA(
						"[FieldScene] "
						"Failed to create Enemy.\n");
				}

				break;

			default:
				break;
			}
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
	m_TurnManager.Initialize();
	m_gameObjectManager.Initialize();

}

void FieldScene::Finalize()
{
	m_TerrainObjects.clear();

	m_TerrainMapData.Clear();
	// GameObjectの終了処理
	m_gameObjectManager.Clear();
}

void FieldScene::Update(
	uint64_t delta)
{
	//=====================================================
	// Mouse Picking
	//=====================================================

	if (INPUT_MANAGER.IsMousePressed(
		MouseButton::Left))
	{
		GameObject* cameraObject =
			m_gameObjectManager.FindByName(
				"MainCamera");

		if (cameraObject)
		{
			auto* camera =
				cameraObject->GetComponent<
				CameraComponent>();

			if (camera)
			{
				const Vector2 mousePosition =
					INPUT_MANAGER.GetMousePosition();

				const Matrix4x4 view =
					camera->GetViewMatrix();

				const Matrix4x4 projection =
					camera->GetProjectionMatrix();

				Vector3 worldPosition(
					0.0f,
					0.0f,
					0.0f);

				if (MousePicker::PickGround(
					mousePosition,
					static_cast<float>(
						Window::GetWidth()),
					static_cast<float>(
						Window::GetHeight()),
					view,
					projection,
					0.0f,
					worldPosition))
				{
					//=====================================
					// World → Grid
					//=====================================

					const GridPosition clickedGrid =
						m_GridMap.WorldToGrid(
							worldPosition);

					//=====================================
					// Grid範囲確認
					//=====================================

					if (!m_GridMap.IsInside(
						clickedGrid))
					{
						return;
					}

					//=====================================
					// Player取得
					//=====================================

					std::vector<GameObject*> players =
						m_gameObjectManager.FindByTag(
							Tag::Player);

					if (players.empty())
					{
						return;
					}

					GameObject* player =
						players[0];

					auto* controller =
						player->GetComponent<
						PlayerControllerComponent>();

					if (!controller)
					{
						return;
					}

					//=====================================
					// Clicked Cell取得
					//=====================================

					GridCell* cell =
						m_GridMap.GetCell(
							clickedGrid);

					if (!cell)
					{
						return;
					}

					GameObject* occupant =
						cell->Occupant;

					//=====================================
					// 空きマス
					//=====================================

					if (!occupant)
					{
						controller->
							RequestMoveTo(
								clickedGrid);

						return;
					}

					//=====================================
					// Enemy
					//=====================================

					if (occupant->GetTag() ==
						Tag::Enemy)
					{
						controller->
							RequestAttack(
								occupant);

						return;
					}

					//=====================================
					// その他
					//=====================================

					OutputDebugStringA(
						"[FieldScene] "
						"Clicked occupied cell.\n");
				}
			}
		}
	}
	//=====================================================
	// Enemy Turn
	//=====================================================
	if (m_TurnManager.IsEnemyTurn())
	{
		UpdateEnemyTurnSequence();
	}

	//=====================================================
	// GameObject Update
	//=====================================================

	m_gameObjectManager.Update(
		delta);

	//=====================================================
	// 帰還判定
	//=====================================================
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

			// ExitのGridへ到達したら自動遷移
			if (playerPosition ==
				exitPosition)
			{
				SceneManager::SetCurrentScene(
					fieldExit->
					GetTargetScene());

				return;
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

		// 既に削除予約済みなら処理しない。
		if (enemy->IsDestroy())
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

		// Questへ討伐通知。
		QUEST_MANAGER.
			NotifyEnemyKilled();

		// GameObjectManagerによる
		// 遅延削除を予約する。
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

	//=================================================
	// Grid
	//=================================================

	GridRenderer::Draw(
		m_GridMap,
		Color(
			1.0f,
			1.0f,
			1.0f,
			0.35f),
		0.02f);

	//=================================================
	// Player Range
	//=================================================

	if (m_TurnManager.IsPlayerTurn())
	{
		std::vector<GameObject*> players =
			m_gameObjectManager.FindByTag(
				Tag::Player);

		if (!players.empty())
		{
			GameObject* player =
				players[0];

			auto* gridPosition =
				player->GetComponent<
				GridPositionComponent>();

			auto* controller =
				player->GetComponent<
				PlayerControllerComponent>();

			if (gridPosition &&
				controller)
			{
				const GridPosition playerGrid =
					gridPosition->
					GetGridPosition();

				const int movePoints =
					controller->
					GetRemainingMovePoints();

				//=====================================
				// Move Range
				//=====================================

				const auto moveRange =
					GridRangeCalculator::
					CalculateMoveRange(
						m_GridMap,
						playerGrid,
						movePoints);

				GridRangeRenderer::DrawCells(
					m_GridMap,
					moveRange,
					Color(
						0.2f,
						0.5f,
						1.0f,
						0.30f),
					0.03f);

				//=====================================
				// Attack Range
				//=====================================

				const auto attackRange =
					GridRangeCalculator::
					CalculateAttackRange(
						m_GridMap,
						playerGrid,
						1);

				GridRangeRenderer::DrawCells(
					m_GridMap,
					attackRange,
					Color(
						1.0f,
						0.2f,
						0.2f,
						0.35f),
					0.04f);
			}
		}
	}

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


void FieldScene::BeginEnemyTurnSequence()
{
	m_EnemyTurnOrder =
		m_gameObjectManager.FindByTag(
			Tag::Enemy);

	m_CurrentEnemyIndex =
		0;

	m_EnemyActionStarted =
		false;

	m_EnemyTurnSequenceActive =
		true;

	//=================================================
	// 各Enemyを次の行動可能状態へ戻す
	//=================================================

	for (GameObject* enemy :
		m_EnemyTurnOrder)
	{
		if (!enemy ||
			enemy->IsDestroy())
		{
			continue;
		}

		auto* enemyAI =
			enemy->GetComponent<
			EnemyAIComponent>();

		if (enemyAI)
		{
			enemyAI->
				ResetAction();
		}
	}
}

void FieldScene::UpdateEnemyTurnSequence()
{
	if (!m_EnemyTurnSequenceActive)
	{
		BeginEnemyTurnSequence();
	}

	//=================================================
	// 全Enemy終了
	//=================================================

	if (m_CurrentEnemyIndex >=
		m_EnemyTurnOrder.size())
	{
		EndEnemyTurnSequence();

		return;
	}

	//=================================================
	// 現在Enemy取得
	//=================================================

	GameObject* enemy =
		m_EnemyTurnOrder[
			m_CurrentEnemyIndex];

	// 無効なEnemyは次へ。
	if (!enemy ||
		enemy->IsDestroy())
	{
		++m_CurrentEnemyIndex;

		m_EnemyActionStarted =
			false;

		return;
	}

	auto* enemyStatus =
		enemy->GetComponent<
		CharacterStatusComponent>();

	if (!enemyStatus ||
		enemyStatus->IsDead())
	{
		++m_CurrentEnemyIndex;

		m_EnemyActionStarted =
			false;

		return;
	}

	auto* enemyAI =
		enemy->GetComponent<
		EnemyAIComponent>();

	if (!enemyAI)
	{
		++m_CurrentEnemyIndex;

		m_EnemyActionStarted =
			false;

		return;
	}

	//=================================================
	// 行動開始
	//=================================================

	if (!m_EnemyActionStarted)
	{
		std::vector<GameObject*> players =
			m_gameObjectManager.FindByTag(
				Tag::Player);

		enemyAI->Act(
			players);

		m_EnemyActionStarted =
			true;
	}

	//=================================================
	// 行動終了待ち
	//=================================================
	//
	// Attack中ならEnemyAI::Update()が
	// Animation終了までFinishedにしない。
	//

	if (!enemyAI->
		IsActionFinished())
	{
		return;
	}

	//=================================================
	// 次のEnemyへ
	//=================================================

	++m_CurrentEnemyIndex;

	m_EnemyActionStarted =
		false;

	// 最後のEnemyまで終わった場合。
	if (m_CurrentEnemyIndex >=
		m_EnemyTurnOrder.size())
	{
		EndEnemyTurnSequence();
	}
}

void FieldScene::EndEnemyTurnSequence()
{
	m_EnemyTurnOrder.clear();

	m_CurrentEnemyIndex =
		0;

	m_EnemyActionStarted =
		false;

	m_EnemyTurnSequenceActive =
		false;

	// Enemy全員の行動完了後にのみ
	// PlayerTurnへ戻す。
	m_TurnManager.
		EndEnemyTurn();
}


// キャラクター生成関数
GameObject* FieldScene::CreatePlayer(
	const CharacterSpawnData& data)
{
	//=================================================
	// 現在対応しているPlayerType確認
	//=================================================

	if (data.Type != "Warrior")
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Unknown Player Type.\n");

		return nullptr;
	}

	auto* warrior =
		m_gameObjectManager.Create<
		GameObject>();

	if (!warrior)
	{
		return nullptr;
	}

	// IDをGameObject名として使用する。
	// MapData上で各Characterを一意に識別できる。
	warrior->SetName(
		data.ID);

	warrior->SetTag(
		Tag::Player);

	//=================================================
	// Transform
	//=================================================

	auto* transform =
		warrior->AddComponent<
		TransformComponent>();

	if (transform)
	{
		transform->SetRotation(
			0.0f,
			3.3f,
			0.0f);

		transform->SetScale(
			0.01f);
	}

	//=================================================
	// Model
	//=================================================

	auto* model =
		warrior->AddComponent<
		ModelComponent>();

	if (model)
	{
		model->SetModel(
			"Assets/Models/Player/Warrior.fbx");

		model->SetTexture(
			0,
			"Assets/Models/Player/Warrior_Texture.png");

		model->SetTexture(
			1,
			"Assets/Models/Player/Warrior_Sword_Texture.png");

		model->PlayAnimation(
			AnimationID::Idle,
			true,
			0.0f);
	}

	//=================================================
	// Audio
	//=================================================

	auto* audioSource =
		warrior->AddComponent<
		AudioSourceComponent>();

	if (audioSource)
	{
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

	//=================================================
	// GridPosition
	//=================================================

	auto* gridPosition =
		warrior->AddComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		warrior->Destroy();

		return nullptr;
	}

	gridPosition->SetGridMap(
		&m_GridMap);

	if (!gridPosition->
		SetGridPosition(
			data.Position))
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Player spawn position is invalid.\n");

		warrior->Destroy();

		return nullptr;
	}

	//=================================================
	// Controller
	//=================================================

	auto* controller =
		warrior->AddComponent<
		PlayerControllerComponent>();

	if (controller)
	{
		controller->SetTurnManager(
			&m_TurnManager);
	}

	//=================================================
	// Status
	//=================================================

	auto* status =
		warrior->AddComponent<
		CharacterStatusComponent>();

	if (status)
	{
		status->SetMaxHP(
			10);

		status->SetAttackPower(
			3);

		status->SetAgility(
			3);
	}

	//=================================================
	// CharacterAnimation
	//=================================================

	auto* characterAnimation =
		warrior->AddComponent<
		CharacterAnimationComponent>();

	if (characterAnimation)
	{
		characterAnimation->
			SetIdleAnimation(
				AnimationID::Idle);

		characterAnimation->
			SetMoveAnimation(
				AnimationID::Walk);

		characterAnimation->
			SetAttackAnimation(
				AnimationID::Attack);

		characterAnimation->
			SetDeathAnimation(
				AnimationID::Death);
	}

	return warrior;
}

GameObject* FieldScene::CreateEnemy(
	const CharacterSpawnData& data)
{
	//=================================================
	// 現在対応しているEnemyType確認
	//=================================================

	if (data.Type != "Ranger")
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Unknown Enemy Type.\n");

		return nullptr;
	}

	auto* enemy =
		m_gameObjectManager.Create<
		GameObject>();

	if (!enemy)
	{
		return nullptr;
	}

	// 敵が複数いても識別できるよう、
	// MapDataのIDを使用する。
	enemy->SetName(
		data.ID);

	enemy->SetTag(
		Tag::Enemy);

	//=================================================
	// Transform
	//=================================================

	auto* transform =
		enemy->AddComponent<
		TransformComponent>();

	if (transform)
	{
		transform->SetRotation(
			0.0f,
			3.3f,
			0.0f);

		transform->SetScale(
			0.01f);
	}

	//=================================================
	// Model
	//=================================================

	auto* model =
		enemy->AddComponent<
		ModelComponent>();

	if (model)
	{
		model->SetModel(
			"Assets/Models/Player/Ranger.fbx");

		model->SetTexture(
			0,
			"Assets/Models/Player/Ranger_Texture.png");

		model->SetTexture(
			1,
			"Assets/Models/Player/Ranger_Bow_Texture.png");

		model->PlayAnimation(
			AnimationID::Idle,
			true,
			0.0f);
	}

	//=================================================
	// GridPosition
	//=================================================

	auto* gridPosition =
		enemy->AddComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		enemy->Destroy();

		return nullptr;
	}

	gridPosition->SetGridMap(
		&m_GridMap);

	if (!gridPosition->
		SetGridPosition(
			data.Position))
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Enemy spawn position is invalid.\n");

		enemy->Destroy();

		return nullptr;
	}

	//=================================================
	// AI
	//=================================================

	enemy->AddComponent<
		EnemyAIComponent>();

	//=================================================
	// Status
	//=================================================

	auto* status =
		enemy->AddComponent<
		CharacterStatusComponent>();

	if (status)
	{
		status->SetMaxHP(
			5);

		status->SetAttackPower(
			2);

		status->SetAgility(
			2);
	}

	//=================================================
	// CharacterAnimation
	//=================================================

	auto* characterAnimation =
		enemy->AddComponent<
		CharacterAnimationComponent>();

	if (characterAnimation)
	{
		characterAnimation->
			SetIdleAnimation(
				AnimationID::Idle);

		characterAnimation->
			SetMoveAnimation(
				AnimationID::Walk);

		characterAnimation->
			SetAttackAnimation(
				AnimationID::Punch);

		characterAnimation->
			SetDeathAnimation(
				AnimationID::Death);
	}

	return enemy;
}

void FieldScene::CreateTerrainTile(
	const TerrainCellData& terrainData)
{
	//=================================================
	// GameObject生成
	//=================================================

	GameObject* terrainObject =
		m_gameObjectManager.Create<
		GameObject>();

	if (!terrainObject)
	{
		return;
	}

	terrainObject->SetName(
		"TerrainTile");

	//=================================================
	// Transform
	//=================================================

	auto* transform =
		terrainObject->
		AddComponent<
		TransformComponent>();

	if (transform)
	{
		Vector3 worldPosition =
			m_GridMap.GridToWorld(
				terrainData.Position);
		worldPosition.y = -1.0f;
		transform->SetPosition(
			worldPosition);

		transform->SetScale(
			Vector3(
				0.01f,
				0.01f,
				0.01f));
	}

	//=================================================
	// Terrain情報
	//=================================================

	auto* terrain =
		terrainObject->
		AddComponent<
		TerrainTileComponent>();

	if (terrain)
	{
		terrain->SetTerrainType(
			terrainData.Type);

		terrain->SetGridPosition(
			terrainData.Position);
	}

	//=================================================
	// Model
	//=================================================

	auto* model =
		terrainObject->
		AddComponent<
		ModelComponent>();

	if (!model)
	{
		return;
	}

	// TerrainTypeに応じて
	// 描画Modelを設定する。
	switch (terrainData.Type)
	{
	case TerrainType::Grass:

		model->SetModel(
			"Assets/Models/field/block-grass-large.fbx");
		model->SetTexture(
			0, "Assets/Models/field/colormap.png");
		break;

	case TerrainType::Dirt:

		model->SetModel(
			"Assets/Models/field/block-grass-large.fbx");
		model->SetTexture(
			0, "Assets/Models/field/colormap.png");
		break;

	case TerrainType::Rock:

		model->SetModel(
			"Assets/Models/field/block-grass-large.fbx");
		model->SetTexture(
			0, "Assets/Models/field/colormap.png");
		break;

	case TerrainType::Water:

		model->SetModel(
			"Assets/Models/field/block-grass-large.fbx");
		model->SetTexture(
			0, "Assets/Models/field/colormap.png");
		break;

	default:
		return;
	}

	//=================================================
	// Terrain一覧へ登録
	//=================================================
	//
	// 将来的な描画距離判定では
	// GameObjectManager全体ではなく
	// Terrainだけを走査できる。
	//=================================================

	m_TerrainObjects.push_back(
		terrainObject);
}