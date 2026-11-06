#include "FieldScene.h"
#include "DebugUI.h"
#include "FieldSceneUI.h"
#include "Renderer.h"

// Component
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "AudioSourceComponent.h"
#include "GridPositionComponent.h"
#include "PlayerControllerComponent.h"
#include "PlayerClassComponent.h"
#include "EnemyAIComponent.h"
#include "CharacterStatusComponent.h"
#include "FieldExitComponent.h"
#include "CharacterAnimationComponent.h"
#include "TerrainTileComponent.h"
#include "UIImageComponent.h"
#include "UIRectComponent.h"
#include "UIButtonComponent.h"
#include "UIFontComponent.h"

#include "WarriorClassComponent.h"
#include "WizardClassComponent.h"
#include "RangerClassComponent.h"
#include "MonkClassComponent.h"
#include "RogueClassComponent.h"
#include "ClericClassComponent.h"

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
#include "SpriteRenderer.h"
#include "FontRenderer.h"

#include "CharaMapData.h"
#include "CharaMapLoader.h"

#include "TerrainMapLoader.h"

FieldScene::FieldScene()
{

}

FieldScene::~FieldScene() =
default;

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

		//=====================================================
		// Field UI
		//=====================================================

		m_FieldUI =
			std::make_unique<
			FieldSceneUI>();

		m_FieldUI->Initialize(
			m_gameObjectManager);

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
	m_SelectedPlayer =
		nullptr;

	m_FocusedPlayer =
		nullptr;

	m_MainCamera =
		nullptr;

	//=================================================
	// UI
	//=================================================

	if (m_FieldUI)
	{
		m_FieldUI->Finalize();

		m_FieldUI.reset();
	}

	m_TerrainObjects.clear();

	m_TerrainMapData.Clear();

	m_gameObjectManager.Clear();
}

void FieldScene::Update(
	uint64_t delta)
{
	//=====================================
	// GameObject Update
	//=====================================

	m_gameObjectManager.Update(
		delta);

	//=====================================
	// UI表示状態更新
	//=====================================
	UpdateNextTurnButton();

	if (m_FieldUI)
	{
		m_FieldUI->Update(
			m_FocusedPlayer);
	}

	//=====================================
	// NextTurn Button
	//=====================================

	if (CheckNextTurnButtonClick())
	{
		// 同じMouse入力を
		// Field操作へ流さない。
		return;
	}

	//=====================================
	// PlayerTurn中の処理
	//=====================================

	if (m_TurnManager.IsPlayerTurn())
	{
		UpdatePlayerInput();
	}

	//=====================================
	// Enemy Turn
	//=====================================

	if (m_TurnManager.IsEnemyTurn())
	{
		UpdateEnemyTurnSequence();
	}

	//=====================================
	// Player Turn End
	//=====================================

	CheckPlayerTurnEnd();

	//=====================================
	// 帰還判定
	//=====================================
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

	//=====================================
	// Enemy死亡処理
	//=====================================
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
	m_gameObjectManager.DrawWorld();

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

	if (m_TurnManager.IsPlayerTurn() &&
		m_SelectedPlayer)
	{
		auto* gridPosition =
			m_SelectedPlayer->GetComponent<
			GridPositionComponent>();

		auto* controller =
			m_SelectedPlayer->GetComponent<
			PlayerControllerComponent>();

		if (gridPosition &&
			controller &&
			controller->IsActionFinished())
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
			if (movePoints > 0)
			{
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
			}

			//=================================================
			// Attack Range
			//=================================================

			auto* playerClass =
				m_SelectedPlayer->
				GetComponent<
				PlayerClassComponent>();

			const int attackDistance =
				playerClass
				? playerClass->
				GetNormalAttackRange()
				: 1;

			const auto attackRange =
				GridRangeCalculator::
				CalculateAttackRange(
					m_GridMap,
					playerGrid,
					attackDistance);

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
	SpriteRenderer::Begin();
	m_gameObjectManager.DrawUI();
	SpriteRenderer::End();

	FontRenderer::Begin();
	m_gameObjectManager.DrawFont();
	FontRenderer::End();

	// デバッグUIの描画
	DebugUI::RegisterDebugFunction([this]()
		{
			// GameObjectのDebugUI描画
			m_gameObjectManager.DrawDebugUI();
		});

}

//====================
// Player処理
//====================
void FieldScene::UpdatePlayerInput()
{
	//=====================================
	// Mouse Picking
	// クリックしたマス目を取得・判定・処理
	//=====================================
	
	if (!INPUT_MANAGER.IsMousePressed(
		MouseButton::Left))
	{
		return;
	}

	// Camera取得
	GameObject* cameraObject =
		m_gameObjectManager.FindByName(
			"MainCamera");
	if (!cameraObject)
	{
		return;
	}

	// CameraComponent取得
	auto* camera =
		cameraObject->GetComponent<
		CameraComponent>();
	if (!camera)
	{
		return;
	}

	// どのセルをクリックしたかの計算処理
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

	if (!MousePicker::PickGround(
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
		return;
	}

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
	// Clicked Cell取得
	//=====================================

	GridCell* cell =
		m_GridMap.GetCell(
			clickedGrid);

	if (!cell)
	{
		return;
	}

	// 選択セルにいるキャラを取得
	GameObject* occupant =
		cell->Occupant;

	//=====================================
	// 1:Player選択
	//=====================================

	if (occupant &&
		occupant->GetTag() ==
		Tag::Player)
	{
		auto* controller =
			occupant->GetComponent<
			PlayerControllerComponent>();

		if (!controller)
		{
			return;
		}

		// 行動中のPlayerは選択しない。
		if (!controller->IsActionFinished())
		{
			return;
		}

		// 操作対象
		m_SelectedPlayer =
			occupant;

		// HUD表示対象
		m_FocusedPlayer =
			occupant;

		OutputDebugStringA(
			"[FieldScene] "
			"Player selected.\n");

		return;
	}

	if (!m_SelectedPlayer)
	{
		return;
	}

	//=====================================
	// 2:実行処理
	//=====================================

	auto* controller =
		m_SelectedPlayer->GetComponent<
		PlayerControllerComponent>();

	if (!controller)
	{
		return;
	}

	//=====================================
	// 空きマス
	//=====================================

	if (!occupant)
	{
		if (controller->RequestMoveTo(
			clickedGrid))
		{
			// 行動を開始時に選択状態を解除する
			m_SelectedPlayer = nullptr;
		}

		return;
	}

	//=====================================
	// Enemy
	//=====================================

	if (occupant->GetTag() ==
		Tag::Enemy)
	{
		if (controller->RequestAttack(
			occupant))
		{
			m_SelectedPlayer = nullptr;
		}

		return;
	}

	//=====================================
	// その他
	//=====================================

	OutputDebugStringA(
		"[FieldScene] "
		"Clicked occupied cell.\n");
}

//====================
// EnemyTurn
//====================
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

	{
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

			if (!controller)
			{
				continue;
			}

			// PlayerTurn開始時に
			// MovePointsと行動状態を明示的に初期化する。
			controller->StartPlayerTurn(
				m_TurnManager.GetTurnCount());
		}

		// 新しいPlayerTurnでは
		// 誰も選択されていない状態にする。
		m_SelectedPlayer =
			nullptr;
	}
}


// キャラクター生成関数
GameObject* FieldScene::CreatePlayer(
	const CharacterSpawnData& data)
{
	//=================================================
	// 現在対応しているPlayerType確認
	//=================================================

	//if (data.Type == "Enemy")
	//{
	//	OutputDebugStringA(
	//		"[FieldScene] "
	//		"Unknown Player Type.\n");

	//	return nullptr;
	//}

	auto* player =
		m_gameObjectManager.Create<
		GameObject>();

	if (!player)
	{
		return nullptr;
	}

	// IDをGameObject名として使用する。
	// MapData上で各Characterを一意に識別できる。
	player->SetName(
		data.ID);

	player->SetTag(
		Tag::Player);

	//=================================================
	// Transform
	//=================================================

	auto* transform =
		player->AddComponent<
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
		player->AddComponent<
		ModelComponent>();

	if (!model)
	{
		player->Destroy();

		return nullptr;
	}

	//=================================================
	// Player Type別 Model設定
	//=================================================

	if (data.Type == "Warrior")
	{
		if (!model->SetModel(
			"Assets/Models/Character/Player/Warrior.fbx"))
		{
			player->Destroy();

			return nullptr;
		}

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Warrior_Texture.png");

		model->SetTexture(
			1,
			"Assets/Models/Character/Player/Warrior_Sword_Texture.png");
	}
	else if (data.Type == "Ranger")
	{
		if (!model->SetModel(
			"Assets/Models/Character/Player/Ranger.fbx"))
		{
			player->Destroy();

			return nullptr;
		}

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Ranger_Texture.png");

		model->SetTexture(
			1,
			"Assets/Models/Character/Player/Ranger_Bow_Texture.png");
	}
	else if (data.Type == "Wizard")
	{
		if (!model->SetModel(
			"Assets/Models/Character/Player/Wizard.fbx"))
		{
			player->Destroy();

			return nullptr;
		}

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Wizard_Texture.png");
	}
	else if (data.Type == "Monk")
	{
		if (!model->SetModel(
			"Assets/Models/Character/Player/Monk.fbx"))
		{
			player->Destroy();

			return nullptr;
		}

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Monk_Texture.png");
	}
	else if (data.Type == "Rogue")
	{
		if (!model->SetModel(
			"Assets/Models/Character/Player/Rogue.fbx"))
		{
			player->Destroy();

			return nullptr;
		}

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Rogue_Texture.png");
	}
	else if (data.Type == "Cleric")
	{
		if (!model->SetModel(
			"Assets/Models/Character/Player/Cleric.fbx"))
		{
			player->Destroy();

			return nullptr;
		}

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Cleric_Texture.png");
	}
	else
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Unknown Player Type.\n");

		player->Destroy();

		return nullptr;
	}

	// 初期Animation
	model->PlayAnimation(
		AnimationID::Idle,
		true,
		0.0f);


	//=================================================
	// Audio
	//=================================================

	auto* audioSource =
		player->AddComponent<
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
		player->AddComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		player->Destroy();

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

		player->Destroy();

		return nullptr;
	}

	//=================================================
	// Controller
	//=================================================

	auto* controller =
		player->AddComponent<
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
		player->AddComponent<
		CharacterStatusComponent>();

	if (status)
	{
		//=================================================
		// Warrior Status
		//=================================================

		status->SetMaxHP(
			10);

		// WarriorもSkill使用を想定し、
		// 少量のMPを持たせておく。
		status->SetMaxMP(
			5);

		status->SetAttackPower(
			3);

		status->SetAgility(
			3);
	}

	//=================================================
	// CharacterAnimation
	//=================================================

	auto* characterAnimation =
		player->AddComponent<
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

	//=================================================
	// Player Class
	//=================================================

	if (data.Type == "Warrior")
	{
		player->AddComponent<
			WarriorClassComponent>();
	}
	else if (data.Type == "Ranger")
	{
		player->AddComponent<
			RangerClassComponent>();
	}
	else if (data.Type == "Wizard")
	{
		player->AddComponent<
			WizardClassComponent>();
	}
	else if (data.Type == "Monk")
	{
		player->AddComponent<
			MonkClassComponent>();
	}
	else if (data.Type == "Rogue")
	{
		player->AddComponent<
			RogueClassComponent>();
	}
	else if (data.Type == "Cleric")
	{
		player->AddComponent<
			ClericClassComponent>();
	}

	return player;
}

GameObject* FieldScene::CreateEnemy(
	const CharacterSpawnData& data)
{
	//=================================================
	// 現在対応しているEnemyType確認
	//=================================================

	if (data.Type != "Enemy")
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
			"Assets/Models/Character/Player/Ranger.fbx");

		model->SetTexture(
			0,
			"Assets/Models/Character/Player/Ranger_Texture.png");

		model->SetTexture(
			1,
			"Assets/Models/Character/Player/Ranger_Bow_Texture.png");

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

/**
 * @brief 操作キャラの行動済み確認
 * @return false:未行動キャラあり
 */
bool FieldScene::AreAllPlayersActed()
{
	std::vector<GameObject*> players =
		m_gameObjectManager.FindByTag(
			Tag::Player);

	if (players.empty())
	{
		return false;
	}

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

		if (!controller)
		{
			continue;
		}

		if (!controller->HasActed())
		{
			return false;
		}
	}

	return true;
}

bool FieldScene::AreAllPlayerActionsFinished() const
{
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

		if (!controller)
		{
			continue;
		}

		if (!controller->
			IsActionFinished())
		{
			return false;
		}
	}

	return true;
}

bool FieldScene::AreAllPlayersOutOfMovePoints()
{
	std::vector<GameObject*> players =
		m_gameObjectManager.FindByTag(
			Tag::Player);

	if (players.empty())
	{
		return false;
	}

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

		if (!controller)
		{
			continue;
		}

		// 1人でも移動力が残っていれば、
		// 強制Turn終了はしない。
		if (controller->
			GetRemainingMovePoints() > 0)
		{
			return false;
		}
	}

	return true;
}

void FieldScene::CheckPlayerTurnEnd()
{
	if (!m_TurnManager.IsPlayerTurn())
	{
		return;
	}

	// Playerの移動・攻撃演出中は
	// Turnを切り替えない。
	if (!AreAllPlayerActionsFinished())
	{
		return;
	}

	// 全Playerが行動不能になった場合のみ
	// 強制的にEnemyTurnへ移行する。
	if (!AreAllPlayersOutOfMovePoints())
	{
		return;
	}

	m_SelectedPlayer =
		nullptr;

	m_TurnManager.EndPlayerTurn();
}

void FieldScene::UpdateNextTurnButton()
{
	if (!m_FieldUI)
	{
		return;
	}

	//=================================================
	// Button表示条件
	//=================================================
	//
	// ・PlayerTurn中
	// ・全Playerが最低1回行動済み
	// ・Animation等が終了済み
	// ・まだ移動力が余っている
	//
	// 全員の移動力が0なら
	// CheckPlayerTurnEnd()側で自動終了する。
	//=================================================

	const bool canEndTurn =
		m_TurnManager.IsPlayerTurn() &&
		AreAllPlayersActed() &&
		AreAllPlayerActionsFinished() &&
		!AreAllPlayersOutOfMovePoints();

	m_FieldUI->
		SetNextTurnVisible(
			canEndTurn);
}

bool FieldScene::
CheckNextTurnButtonClick()
{
	if (!m_FieldUI)
	{
		return false;
	}

	if (!m_FieldUI->
		IsNextTurnClicked())
	{
		return false;
	}

	//=================================================
	// Gameplay側でも条件を再確認
	//=================================================
	//
	// UIのEnabled状態だけにGameRuleを依存させない。
	//=================================================

	if (!m_TurnManager.IsPlayerTurn() ||
		!AreAllPlayersActed() ||
		!AreAllPlayerActionsFinished())
	{
		return false;
	}

	m_SelectedPlayer =
		nullptr;

	// HUDは残す。
	// EnemyTurn中は行動済みPlayerが暗く見える。
	//
	// m_FocusedPlayer = nullptr;
	// にはしない。

	m_TurnManager.EndPlayerTurn();

	return true;
}