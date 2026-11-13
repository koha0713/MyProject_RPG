#include "FieldScene.h"
#include "DebugUI.h"
#include "FieldSceneUI.h"
#include "Renderer.h"

#include <cmath>

// Component
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "AudioSourceComponent.h"
#include "GridPositionComponent.h"
#include "EnemyAIComponent.h"
#include "FieldExitComponent.h"
#include "CharacterAnimationComponent.h"
#include "TerrainTileComponent.h"

// Player関係
#include "CharacterStatusComponent.h"
#include "PlayerActionType.h"
#include "PlayerControllerComponent.h"
#include "PlayerClassComponent.h"
#include "PlayerActionComponent.h"

// Battle関係
#include "TargetingSystem.h"

#include "WarriorClassComponent.h"
#include "WizardClassComponent.h"
#include "RangerClassComponent.h"
#include "MonkClassComponent.h"
#include "RogueClassComponent.h"
#include "ClericClassComponent.h"

// UI関係
#include "UIImageComponent.h"
#include "UIRectComponent.h"
#include "UIButtonComponent.h"
#include "UIFontComponent.h"

#include <ModelLoader.h>
#include <DirectXMath.h>
#include "MousePicker.h"
#include "Window.h"

#include "InputManager.h"
#include "SoundManager.h"
#include "QuestManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
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
		20,
		20,
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
			/*CreateTerrainTile(
				cell);*/
		}
	}

	{
		//=====================================================
		// Terrain Renderer
		//=====================================================

		GameObject* terrainObject =
			m_gameObjectManager.
			Create<GameObject>();

		terrainObject->
			SetName(
				"TerrainRenderer");

		m_TerrainRenderer =
			terrainObject->
			AddComponent<
			TerrainRendererComponent>();

		m_TerrainRenderer->
			SetTerrainData(
				&m_TerrainMapData);

		//=====================================================
		// Cell Size
		//=====================================================
		//
		// 必ず既存GridMapと同じ値にする。
		// 1.0fは例。
		//=====================================================

		m_TerrainRenderer->
			SetCellSize(
				2.0f);

		if (!m_TerrainRenderer->
			BuildTerrain())
		{
			OutputDebugStringA(
				"[FieldScene] "
				"TerrainRenderer Build Failed.\n");
		}

		//=====================================================
		// Water Normal Map
		//=====================================================
		//
		// ResourceManager側で同一パスのTextureはキャッシュされるため、
		// 毎回新規ロードされるわけではない。
		//=====================================================

		std::shared_ptr<Texture> waterNormal =
			RESOURCE_MANAGER.LoadTexture(
				"Assets/Textures/Terrain/WaterNormal_1.png");

		// 読み込み失敗時もnullptrを渡せる。
		// Renderer側でIsValid()を確認する設計。
		m_TerrainRenderer->
			SetWaterNormalMaps(
				waterNormal,
				waterNormal);

		auto grassTexture =
			RESOURCE_MANAGER.LoadTexture(
				"Assets/Textures/Terrain/Grass01.png");

		auto dirtTexture =
			RESOURCE_MANAGER.LoadTexture(
				"Assets/Textures/Terrain/Sand01.png");

		auto rockTexture =
			RESOURCE_MANAGER.LoadTexture(
				"Assets/Textures/Terrain/Rock01.png");

		auto sandTexture =
			RESOURCE_MANAGER.LoadTexture(
				"Assets/Textures/Terrain/Sand01.png");

		m_TerrainRenderer->
			SetTerrainTextures(
				grassTexture,
				dirtTexture,
				rockTexture,
				sandTexture);

		GameObject* grassObject =
			m_gameObjectManager.
			Create<GameObject>();

		grassObject->
			SetName(
				"GrassRenderer");

		auto grassTexture_1 =
			RESOURCE_MANAGER.LoadTexture(
				"Assets/Textures/Terrain/GrassBlade.png");

		m_GrassRenderer =
			grassObject->
			AddComponent<GrassRendererComponent>();

		m_GrassRenderer->
			SetCellSize(
				2.0f);

		m_GrassRenderer->
			SetTerrainData(
				&m_TerrainMapData);

		m_GrassRenderer->
			SetGrassTexture(
				grassTexture_1);

		m_GrassRenderer->
			BuildGrass(
				m_TerrainRenderer->
				GetChunks());
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
						28.0f,
						10.0f,
						23.0f);

					// 原点方向を向く初期値
					cameraTransform->SetRotation(
						-0.9f,
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

		m_RadialMenuUI.Initialize(
			m_gameObjectManager);

		//====================
		// Directional Light
		//====================
		{
			DirectionalLight directionalLight;

			directionalLight.Direction =
				Vector3(
					0.35f,
					-1.0f,
					0.3f);

			directionalLight.LightColor =
				Color(
					1.0f,
					0.97f,
					0.90f,
					1.0f);

			directionalLight.Intensity =
				0.85f;

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
				0.65f;

			toon.ShadowThreshold =
				0.30f;

			// 日中なので中間色はかなり明るめ
			toon.MidToneIntensity =
				0.92f;

			// 影でも色が潰れない程度
			toon.ShadowIntensity =
				0.65f;

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
				14,
				8
			});

		transform->SetPosition(
			14.0f * 2.0f,
			0.0f,
			8.0f * 2.0f);

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

	m_RadialMenuUI.Finalize();

	m_TerrainObjects.clear();

	m_TerrainMapData.Clear();

	m_gameObjectManager.Clear();
}

void FieldScene::Update(
	uint64_t delta)
{
	//=================================================
	// Input取得
	//=================================================

	const Vector2& mousePosition =
		INPUT_MANAGER.
		GetMousePosition();

	const bool leftHeld =
		INPUT_MANAGER.
		IsMouseDown(
			MouseButton::Left);

	const bool leftReleased =
		INPUT_MANAGER.
		IsMouseReleased(
			MouseButton::Left);

	const bool rightPressed =
		INPUT_MANAGER.
		IsMousePressed(
			MouseButton::Right);

	const bool escapePressed =
		INPUT_MANAGER.
		IsKeyPressed(
			KeyCode::Escape);

	// 地形LOD表示用
	if (m_TerrainRenderer)
	{
		m_TerrainRenderer->SetCameraPosition(
			m_MainCamera->
			GetOwner()->
			GetComponent<
			TransformComponent>()->
			GetPosition());
	}

	//=================================================
	// 通常Player入力を止めるか
	//=================================================
	//
	// RadialMenuやTargetSelection中に、
	// 既存のUpdatePlayerInput()が同時に動くと
	// 移動・攻撃などが誤発動する可能性がある。
	//=================================================

	bool blockNormalPlayerInput =
		false;

	//=================================================
	// Player Action Cancel
	//=================================================
	//
	// RadialMenu / TargetSelection中は
	// 右クリックまたはEscでキャンセル。
	//
	// Update全体は止めず、
	// このFrameの通常Player入力だけ止める。
	//=================================================

	if (rightPressed ||
		escapePressed)
	{
		if (m_PlayerInputMode ==
			PlayerInputMode::RadialMenu ||
			m_PlayerInputMode ==
			PlayerInputMode::TargetSelection)
		{
			CancelPlayerActionSelection();

			blockNormalPlayerInput =
				true;
		}
	}

	//=================================================
	// Radial Menu Input
	//=================================================

	if (m_RadialPressActive)
	{
		UpdateRadialInput(
			delta,
			mousePosition.x,
			mousePosition.y,
			leftHeld,
			leftReleased);

		// Radial操作開始後は、
		// 既存Player入力と競合させない。
		blockNormalPlayerInput =
			true;
	}

	//=================================================
	// Radial / Target Selection中
	//=================================================
	//
	// Mouseを離したFrameで
	// BeginActionSelection()によって
	// TargetSelectionへ移行する場合がある。
	//
	// その場合も通常Player入力は止める。
	//=================================================

	if (m_PlayerInputMode ==
		PlayerInputMode::RadialMenu ||
		m_PlayerInputMode ==
		PlayerInputMode::TargetSelection ||
		m_PlayerInputMode ==
		PlayerInputMode::Executing)
	{
		blockNormalPlayerInput =
			true;
	}

	//=====================================
	// GameObject Update
	//=====================================

	m_gameObjectManager.Update(
		delta);

	//=================================================
	// Action Execution State
	//=================================================
	//
	// Controller側でActionが完了したら、
	// FieldSceneのInputModeも通常状態へ戻す。
	//=================================================
	if (m_PlayerInputMode ==
		PlayerInputMode::Executing)
	{
		if (!m_SelectedPlayer ||
			m_SelectedPlayer->
			IsDestroy())
		{
			m_PlayerInputMode =
				PlayerInputMode::None;

			m_SelectedPlayer =
				nullptr;
		}
		else
		{
			auto* controller =
				m_SelectedPlayer->
				GetComponent<
				PlayerControllerComponent>();

			if (!controller ||
				controller->
				IsActionFinished())
			{
				m_PlayerInputMode =
					PlayerInputMode::None;

				// HUD表示対象は残すが、
				// 操作対象選択は一度解除する。
				m_SelectedPlayer =
					nullptr;
			}
		}
	}

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
		// Action中の手動Turn終了は禁止。
		if (AreAllPlayerActionsFinished())
		{
			m_SelectedPlayer =
				nullptr;

			// Player操作状態もResetする。
			CancelPlayerActionSelection();

			m_TurnManager.
				EndPlayerTurn();

			BeginEnemyTurnSequence();
		}
	}

	//=====================================
	// PlayerTurn中の処理
	//=====================================

	if (m_TurnManager.IsPlayerTurn())
	{
		//=================================================
		// Target Selection
		//=================================================

		if (m_PlayerInputMode ==
			PlayerInputMode::TargetSelection)
		{
			UpdateTargetSelection();
		}

		//=================================================
		// Normal Input
		//=================================================

		else if (!blockNormalPlayerInput)
		{
			UpdatePlayerInput();
		}
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
		for( GameObject* player : players)
		{
			if (!player ||
				player->IsDestroy())
			{
				continue;
			}
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
					playerGrid->
					GetGridPosition();
				const GridPosition exitPosition =
					fieldExit->
					GetGridPosition();
				// ExitのGridへ到達したら自動遷移
				if (playerPosition ==
					exitPosition)
				{
					auto* audioSource =
						player->
						GetComponent<
						AudioSourceComponent>();
					if (audioSource)
					{
						audioSource->SetSoundPath(
							L"Assets/Sounds/SE/決定ボタンを押す2.wav");
						audioSource->
							PlayOneShot(
								1.0f);
					}
					SceneManager::SetCurrentScene(
						fieldExit->
						GetTargetScene());
					return;
				}
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
			0.1f),
		0.02f);

	//=================================================
	// Selected Action Range
	//=================================================
	//
	// 通常時はRangeを表示しない。
	//
	// RadialMenuからActionを選択し、
	// TargetSelection状態になった場合のみ
	// 選択中ActionのRangeを表示する。
	//=================================================

	if (m_TurnManager.IsPlayerTurn() &&
		m_SelectedPlayer &&
		m_PlayerInputMode ==
		PlayerInputMode::TargetSelection)
	{
		auto* gridPosition =
			m_SelectedPlayer->GetComponent<
			GridPositionComponent>();

		auto* controller =
			m_SelectedPlayer->GetComponent<
			PlayerControllerComponent>();

		auto* status =
			m_SelectedPlayer->GetComponent<
			CharacterStatusComponent>();

		auto* playerAction =
			m_SelectedPlayer->GetComponent<
			PlayerActionComponent>();

		//=================================================
		// 必要Component確認
		//=================================================
		if (gridPosition &&
			controller &&
			status &&
			playerAction &&
			controller->IsActionFinished())
		{
			const GridPosition playerGrid =
				gridPosition->
				GetGridPosition();

			//=================================================
			// Action使用可能確認
			//=================================================

			if (playerAction->
				CanUseAction(
					m_SelectedAction))
			{
				const PlayerActionDefinition definition =
					playerAction->
					GetActionDefinition(
						m_SelectedAction);

				if (definition.Enabled)
				{
					switch (m_SelectedAction)
					{
						//=========================================
						// Move
						//=========================================

					case PlayerActionType::Move:
					{
						const int moveRangeDistance =
							status->
							CalculateMoveRange();

						if (moveRangeDistance > 0)
						{
							const auto moveRange =
								GridRangeCalculator::
								CalculateMoveRange(
									m_GridMap,
									playerGrid,
									moveRangeDistance);

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

						break;
					}

					//=========================================
					// Targeting Action
					//=========================================

					case PlayerActionType::NormalAttack:
					case PlayerActionType::Skill1:
					case PlayerActionType::Skill2:
					case PlayerActionType::Ultimate:
					{
						//=====================================
						// Unit / Grid Target
						//=====================================
						//
						// TargetPatternを持つActionは、
						// TargetingSystemで実際に選択可能な
						// Cellを取得する。
						//=====================================

						if ((definition.TargetMode ==
							TargetingMode::Unit ||
							definition.TargetMode ==
							TargetingMode::Grid) &&
							definition.TargetPattern.IsValid())
						{
							const auto targetRange =
								TargetingSystem::
								GetTargetableCells(
									m_GridMap,
									m_SelectedPlayer,
									playerGrid,
									definition.TargetPattern,
									definition.Targeting);

							GridRangeRenderer::DrawCells(
								m_GridMap,
								targetRange,
								Color(
									1.0f,
									0.2f,
									0.2f,
									0.35f),
								0.04f);
						}

						//=====================================
						// Direction Target
						//=====================================
						//
						// Mouse位置から4方向を決定し、
						// EffectPatternそのものをPreviewする。
						//
						// Enemyの有無に関係なく表示する。
						//=====================================

						if (definition.TargetMode ==
							TargetingMode::Direction &&
							definition.HasEffectPattern &&
							definition.EffectPattern.IsValid())
						{
							GridPosition mouseGrid
							{
								0,
								0
							};

							if (TryGetMouseGrid(
								mouseGrid))
							{
								GridDirection direction =
									GridDirection::North;

								if (CalculateGridDirection(
									playerGrid,
									mouseGrid,
									direction))
								{
									const auto offsets =
										definition.EffectPattern.
										GetOffsets(
											direction);

									std::vector<GridPosition>
										effectCells;

									for (const GridOffset& offset :
										offsets)
									{
										const GridPosition cellPosition
										{
											playerGrid.X +
												offset.X,

											playerGrid.Y +
												offset.Y
										};

										// Map外は描画しない。
										if (!m_GridMap.IsInside(
											cellPosition))
										{
											continue;
										}

										effectCells.push_back(
											cellPosition);
									}

									GridRangeRenderer::DrawCells(
										m_GridMap,
										effectCells,
										Color(
											1.0f,
											0.35f,
											0.1f,
											0.40f),
										0.05f);
								}
							}
						}

						break;
					}

					case PlayerActionType::Item:
					{
						// Inventory実装後に対応。
						break;
					}

					default:
						break;
					}
				}
			}
		}
	}

	SpriteRenderer::Begin();
	m_gameObjectManager.DrawUI();
	SpriteRenderer::End();

	FontRenderer::Begin();
	m_gameObjectManager.DrawFont();
	FontRenderer::End();

	//// デバッグUIの描画
	//DebugUI::RegisterDebugFunction([this]()
	//	{
	//		// GameObjectのDebugUI描画
	//		m_gameObjectManager.DrawDebugUI();
	//	});

}

//====================
// Player処理
//====================
void FieldScene::UpdatePlayerInput()
{
	if (!INPUT_MANAGER.IsMousePressed(
		MouseButton::Left))
	{
		return;
	}

	const Vector2 mousePosition =
		INPUT_MANAGER.
		GetMousePosition();

	GridPosition clickedGrid
	{
		0,
		0
	};

	if (!TryGetMouseGrid(
		clickedGrid))
	{
		return;
	}

	GridCell* cell =
		m_GridMap.GetCell(
			clickedGrid);

	if (!cell)
	{
		return;
	}

	GameObject* occupant =
		cell->Occupant;

	//=================================================
	// Player以外は通常状態では処理しない
	//=================================================
	//
	// 空きマス / Enemyは、
	// Action選択後のTargetSelectionで処理する。
	//=================================================

	if (!occupant ||
		occupant->GetTag() !=
		Tag::Player)
	{
		return;
	}

	//=================================================
	// Player Controller
	//=================================================

	auto* controller =
		occupant->GetComponent<
		PlayerControllerComponent>();

	if (!controller)
	{
		return;
	}

	// 行動中Playerからは
	// RadialMenuを開かない。
	if (!controller->
		IsActionFinished())
	{
		return;
	}

	//=================================================
	// Player Selection
	//=================================================

	// 操作対象。
	m_SelectedPlayer =
		occupant;

	// HUD表示対象。
	m_FocusedPlayer =
		occupant;

	//=================================================
	// Radial Press開始
	//=================================================
	//
	// この時点ではMenuは表示しない。
	//
	// UpdateRadialInput()で
	// m_RadialHoldThresholdを超えた時点で
	// OpenRadialMenu()される。
	//=================================================

	BeginRadialPress(
		mousePosition.x,
		mousePosition.y);

	OutputDebugStringA(
		"[FieldScene] "
		"Player selected. "
		"Radial press started.\n");
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
		// Class Status
		//=================================================

		if (data.Type == "Warrior")
		{
			status->SetMaxHP(120);
			status->SetMaxMP(25);
			status->SetMaxAP(4);

			status->SetStrength(8);
			status->SetDexterity(5);
			status->SetAgility(4);
			status->SetPerception(4);

			status->SetBaseMoveRange(2);

			// 暫定Damage値。
			status->SetAttackPower(3);
		}
		else if (data.Type == "Wizard")
		{
			status->SetMaxHP(70);
			status->SetMaxMP(120);
			status->SetMaxAP(4);

			status->SetStrength(2);
			status->SetDexterity(4);
			status->SetAgility(3);
			status->SetPerception(6);

			status->SetBaseMoveRange(2);

			status->SetAttackPower(3);
		}
		else if (data.Type == "Ranger")
		{
			status->SetMaxHP(85);
			status->SetMaxMP(45);
			status->SetMaxAP(4);

			status->SetStrength(4);
			status->SetDexterity(9);
			status->SetAgility(7);
			status->SetPerception(9);

			status->SetBaseMoveRange(3);

			status->SetAttackPower(3);
		}
		else if (data.Type == "Monk")
		{
			status->SetMaxHP(105);
			status->SetMaxMP(35);
			status->SetMaxAP(4);

			status->SetStrength(7);
			status->SetDexterity(7);
			status->SetAgility(8);
			status->SetPerception(5);

			status->SetBaseMoveRange(3);

			status->SetAttackPower(2);
		}
		else if (data.Type == "Rogue")
		{
			status->SetMaxHP(75);
			status->SetMaxMP(40);
			status->SetMaxAP(4);

			status->SetStrength(5);
			status->SetDexterity(9);
			status->SetAgility(10);
			status->SetPerception(7);

			status->SetBaseMoveRange(4);

			status->SetAttackPower(3);
		}
		else if (data.Type == "Cleric")
		{
			status->SetMaxHP(90);
			status->SetMaxMP(100);
			status->SetMaxAP(4);

			status->SetStrength(3);
			status->SetDexterity(5);
			status->SetAgility(4);
			status->SetPerception(8);

			status->SetBaseMoveRange(2);

			status->SetAttackPower(2);
		}
	}

	//=================================================
	// Player Action
	//=================================================

	auto* playerAction =
		player->AddComponent<
		PlayerActionComponent>();

	if (!playerAction)
	{
		player->Destroy();

		return nullptr;
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
		worldPosition.y = 0.0f;
		transform->SetPosition(
			worldPosition);

		transform->SetScale(
			Vector3(
				0.5f,
				3.0f,
				0.5f));
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
			"Assets/Models/field/Grass.obj");
		model->SetTexture(
			0, "Assets/Models/field/Grass_Albedo.png");
		break;

	case TerrainType::Dirt:
	{
		const uint8_t connection =
			terrainData.RoadConnections;

		const char* modelPath = nullptr;
		const char* texturePath = nullptr;

		float rotationY = 0.0f;

		switch (connection)
		{
			// 終端：上を基準
		case 0:
		case 1:
			modelPath = "Assets/Models/field/Road_End.obj";
			texturePath = "Assets/Models/field/Road_End_Albedo.png";
			rotationY = 180.0f;
			break;

		case 2:
			modelPath = "Assets/Models/field/Road_End.obj";
			texturePath = "Assets/Models/field/Road_End_Albedo.png";
			rotationY = 90.0f;
			break;

		case 4:
			modelPath = "Assets/Models/field/Road_End.obj";
			texturePath = "Assets/Models/field/Road_End_Albedo.png";
			break;

		case 8:
			modelPath = "Assets/Models/field/Road_End.obj";
			texturePath = "Assets/Models/field/Road_End_Albedo.png";
			rotationY = 270.0f;
			break;

			// 直線
		case 5:
		case 10:
			modelPath = "Assets/Models/field/Road_Straight.obj";
			texturePath = "Assets/Models/field/Road_Straight_Albedo.png";
			rotationY = (connection == 10) ? -90.0f : 0.0f;
			break;

			// 曲がり角：上＋右を基準
		case 3:
		case 6:
		case 12:
		case 9:
			modelPath = "Assets/Models/field/Road_Corner.obj";
			texturePath = "Assets/Models/field/Road_Corner_Albedo.png";

			switch (connection)
			{
			case 3:  rotationY = 90.0f; break;
			case 12: rotationY = 270.0f; break;
			case 9:  rotationY = 180.0f; break;
			default: break;
			}
			break;

			// T字路：上＋右＋左を基準
		case 11:
		case 7:
		case 14:
		case 13:
			modelPath = "Assets/Models/field/Road_T.obj";
			texturePath = "Assets/Models/field/Road_T_Albedo.png";

			switch (connection)
			{
			case 7:  rotationY = 90.0f; break;
			case 11: rotationY = 180.0f; break;
			case 13: rotationY = 270.0f; break;
			default: break;
			}
			break;

			// 十字路
		case 15:
			modelPath = "Assets/Models/field/Road_Cross.obj";
			texturePath = "Assets/Models/field/Road_Cross_Albedo.png";
			break;

		default:
			break;
		}

		if (!modelPath || !texturePath)
		{
			break;
		}

		//=============================================
		// モデルとテクスチャの設定
		//=============================================

		model->SetModel(modelPath);
		model->SetTexture(0, texturePath);

		//=============================================
		// 回転設定
		//=============================================

		if (transform)
		{
			// 度数からラジアンに変換する。
			transform->SetRotation(
				Vector3(
					0.0f,
					DirectX::XMConvertToRadians(rotationY),
					0.0f));
		}

		break;
	}
	case TerrainType::Rock:

		model->SetModel(
			"Assets/Models/field/Rock.obj");
		model->SetTexture(
			0, "Assets/Models/field/Rock_Albedo.png");
		break;
	case TerrainType::Sand:
		model->SetModel(
			"Assets/Models/field/Sand.obj");
		model->SetTexture(
			0, "Assets/Models/field/Sand_Albedo.png");
		break;

	case TerrainType::Water:

		model->SetModel(
			"Assets/Models/field/Water.obj");
		model->SetTexture(
			0, "Assets/Models/field/Water_Albedo.png");
		break;

	case TerrainType::Forest:
		model->SetModel(
			"Assets/Models/field/Forest.obj");
		model->SetTexture(
			0, "Assets/Models/field/Forest_Albedo.png");
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

//=====================================================
// 全Player Action終了確認
//=====================================================
bool FieldScene::AreAllPlayerActionsFinished() const
{
	const auto players =
		m_gameObjectManager.FindByTag(
			Tag::Player);

	for (GameObject* player :
		players)
	{
		if (!player ||
			player->IsDestroy())
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

//=====================================================
// 全Player AP消費確認
//=====================================================

bool FieldScene::AreAllPlayersOutOfAP() const
{
	const auto players =
		m_gameObjectManager.FindByTag(
			Tag::Player);

	// Playerが存在しない場合は
	// 自動Turn終了させない。
	if (players.empty())
	{
		return false;
	}

	bool foundAlivePlayer =
		false;

	for (GameObject* player :
		players)
	{
		if (!player ||
			player->IsDestroy())
		{
			continue;
		}

		auto* status =
			player->GetComponent<
			CharacterStatusComponent>();

		if (!status)
		{
			continue;
		}

		// 戦闘不能Characterは
		// 行動可能Playerとして扱わない。
		if (status->IsDead())
		{
			continue;
		}

		foundAlivePlayer =
			true;

		// 1APでも残っているPlayerがいるなら
		// 全員AP切れではない。
		if (status->GetAP() > 0)
		{
			return false;
		}
	}

	// 生存Playerが存在し、
	// 全員AP 0ならtrue。
	return foundAlivePlayer;
}

//=====================================================
// Player Turn終了判定
//=====================================================
void FieldScene::CheckPlayerTurnEnd()
{
	// PlayerTurn以外では判定しない。
	if (!m_TurnManager.IsPlayerTurn())
	{
		return;
	}

	//=================================================
	// 実行中Action確認
	//=================================================
	//
	// APが0でも、
	// Move / Attack Animationが残っている間は
	// EnemyTurnへ進めない。
	//=================================================

	if (!AreAllPlayerActionsFinished())
	{
		return;
	}

	//=================================================
	// AP確認
	//=================================================

	if (!AreAllPlayersOutOfAP())
	{
		return;
	}

	//=================================================
	// Player Turn End
	//=================================================

	m_SelectedPlayer =
		nullptr;

	m_TurnManager.
		EndPlayerTurn();

	// Enemy行動開始。
	BeginEnemyTurnSequence();
}

void FieldScene::UpdateNextTurnButton()
{
	const bool canEndTurn =
		m_TurnManager.IsPlayerTurn() &&
		AreAllPlayerActionsFinished();

	if (m_FieldUI)
	{
		m_FieldUI->
			SetNextTurnVisible(
				canEndTurn);
	}
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

void FieldScene::BeginRadialPress(
	float mouseX,
	float mouseY)
{
	if (!m_TurnManager.
		IsPlayerTurn())
	{
		return;
	}

	if (!m_SelectedPlayer)
	{
		return;
	}

	auto* controller =
		m_SelectedPlayer->
		GetComponent<
		PlayerControllerComponent>();

	if (!controller ||
		!controller->
		IsActionFinished())
	{
		return;
	}

	//=================================================
	// Press開始
	//=================================================

	m_RadialPressActive =
		true;

	m_RadialHoldTime =
		0.0f;

	m_RadialOriginX =
		mouseX;

	m_RadialOriginY =
		mouseY;

	m_HasRadialAction =
		false;

	// この段階ではまだ表示しない。
	// HoldThresholdを超えた時点で開く。
}

//=====================================================
// Update Radial Input
//=====================================================
void FieldScene::UpdateRadialInput(
	uint64_t delta,
	float mouseX,
	float mouseY,
	bool leftHeld,
	bool leftReleased)
{
	if (!m_RadialPressActive)
	{
		return;
	}

	//=================================================
	// Hold
	//=================================================

	const float deltaSeconds =
		static_cast<float>(
			delta) *
		0.001f;

	// Menuがまだ開いていない段階だけ
	// Hold時間を加算する。
	if (m_PlayerInputMode !=
		PlayerInputMode::RadialMenu &&
		leftHeld)
	{
		m_RadialHoldTime +=
			deltaSeconds;
	}

	//=================================================
	// Menu Open
	//=================================================

	if (m_PlayerInputMode !=
		PlayerInputMode::RadialMenu &&
		m_RadialHoldTime >=
		m_RadialHoldThreshold)
	{
		OpenRadialMenu();
	}

	//=================================================
	// Radial Highlight
	//=================================================

	if (m_PlayerInputMode ==
		PlayerInputMode::RadialMenu)
	{
		const float deltaX =
			mouseX -
			m_RadialOriginX;

		const float deltaY =
			mouseY -
			m_RadialOriginY;

		PlayerActionType action =
			PlayerActionType::Move;

		if (CalculateRadialAction(
			deltaX,
			deltaY,
			action))
		{
			m_HasRadialAction =
				true;

			m_RadialAction =
				action;

			m_RadialMenuUI.
				SetHighlightedAction(
					action);
		}
		else
		{
			m_HasRadialAction =
				false;

			m_RadialMenuUI.
				ClearHighlight();
		}
	}

	//=================================================
	// Release
	//=================================================

	if (!leftReleased)
	{
		return;
	}

	if (m_PlayerInputMode ==
		PlayerInputMode::RadialMenu)
	{
		ConfirmRadialAction();

		return;
	}

	//=================================================
	// Menuが開く前にMouseを離した
	//=================================================

	m_RadialPressActive =
		false;

	m_RadialHoldTime =
		0.0f;
}

void FieldScene::OpenRadialMenu()
{
	if (!m_SelectedPlayer)
	{
		return;
	}

	m_PlayerInputMode =
		PlayerInputMode::RadialMenu;

	m_HasRadialAction =
		false;

	m_RadialMenuUI.
		SetCenter(
			m_RadialOriginX,
			m_RadialOriginY);

	m_RadialMenuUI.
		ClearHighlight();

	m_RadialMenuUI.
		SetVisible(
			true);
}

void FieldScene::CloseRadialMenu()
{
	//=================================================
	// Radial UIを閉じる
	//=================================================

	m_RadialMenuUI.
		SetVisible(
			false);

	m_RadialMenuUI.
		ClearHighlight();

	//=================================================
	// Radial入力状態をReset
	//=================================================

	m_RadialPressActive =
		false;

	m_RadialHoldTime =
		0.0f;

	m_HasRadialAction =
		false;

	// RadialMenu状態なら通常状態へ戻す。
	if (m_PlayerInputMode ==
		PlayerInputMode::RadialMenu)
	{
		m_PlayerInputMode =
			PlayerInputMode::None;
	}
}

//=====================================================
// Confirm Radial Action
//=====================================================
void FieldScene::ConfirmRadialAction()
{
	//=================================================
	// Dead Zone
	//=================================================

	if (!m_HasRadialAction)
	{
		CloseRadialMenu();

		return;
	}

	const PlayerActionType action =
		m_RadialAction;

	//=================================================
	// Action選択
	//=================================================

	BeginActionSelection(
		action);

	//=================================================
	// RadialMenu終了
	//=================================================
	//
	// TargetSelectionへ進んだ場合でも、
	// 選択不可でNoneへ戻った場合でも、
	// RadialMenu自体は必ず閉じる。
	//=================================================

	m_RadialMenuUI.
		SetVisible(
			false);

	m_RadialMenuUI.
		ClearHighlight();

	m_RadialPressActive =
		false;

	m_RadialHoldTime =
		0.0f;

	m_HasRadialAction =
		false;
}

bool FieldScene::CalculateRadialAction(
	float deltaX,
	float deltaY,
	PlayerActionType& outAction) const
{
	//=================================================
	// Dead Zone
	//=================================================

	const float distanceSquared =
		deltaX * deltaX +
		deltaY * deltaY;

	const float deadZoneSquared =
		m_RadialDeadZone *
		m_RadialDeadZone;

	if (distanceSquared <
		deadZoneSquared)
	{
		return false;
	}

	//=================================================
	// Angle
	//=================================================
	//
	// Screen座標は
	// +X = 右
	// +Y = 下
	//
	// atan2(-Y, X) にすることで
	// 上方向を通常の数学座標として扱う。
	//=================================================

	const float angle =
		std::atan2(
			-deltaY,
			deltaX);

	constexpr float Pi =
		3.14159265358979323846f;

	constexpr float TwoPi =
		Pi * 2.0f;

	//=================================================
	// 上方向をSector 0にする
	//=================================================
	//
	// atan2:
	//
	// 右 = 0
	// 上 = +PI/2
	//
	// 上を0度に変換する。
	//=================================================

	float radialAngle =
		(Pi * 0.5f) -
		angle;

	if (radialAngle <
		0.0f)
	{
		radialAngle +=
			TwoPi;
	}

	if (radialAngle >=
		TwoPi)
	{
		radialAngle -=
			TwoPi;
	}

	//=================================================
	// 6 Sector
	//=================================================
	//
	// 1Sector = 60度。
	//
	// 境界を±30度にするため
	// 半Sector分ずらしてからindex化する。
	//=================================================

	constexpr float sectorSize =
		TwoPi /
		6.0f;

	const float shiftedAngle =
		radialAngle +
		sectorSize *
		0.5f;

	const int sector =
		static_cast<int>(
			shiftedAngle /
			sectorSize) %
		6;

	//=================================================
	// Clockwise
	//=================================================
	//
	// 0 : Move
	// 1 : NormalAttack
	// 2 : Skill1
	// 3 : Skill2
	// 4 : Ultimate
	// 5 : Item
	//=================================================

	switch (sector)
	{
	case 0:
		outAction =
			PlayerActionType::Move;
		break;

	case 1:
		outAction =
			PlayerActionType::NormalAttack;
		break;

	case 2:
		outAction =
			PlayerActionType::Skill1;
		break;

	case 3:
		outAction =
			PlayerActionType::Skill2;
		break;

	case 4:
		outAction =
			PlayerActionType::Ultimate;
		break;

	case 5:
		outAction =
			PlayerActionType::Item;
		break;

	default:
		return false;
	}

	return true;
}

//=====================================================
// Begin Action Selection
//=====================================================
void FieldScene::BeginActionSelection(
	PlayerActionType action)
{
	if (!m_SelectedPlayer)
	{
		CloseRadialMenu();

		return;
	}

	auto* playerAction =
		m_SelectedPlayer->
		GetComponent<
		PlayerActionComponent>();

	auto* controller =
		m_SelectedPlayer->
		GetComponent<
		PlayerControllerComponent>();

	if (!playerAction ||
		!controller)
	{
		CloseRadialMenu();

		return;
	}

	//=================================================
	// Action使用可能確認
	//=================================================
	//
	// AP / MP不足の場合も
	// TargetSelectionへは進まない。
	//=================================================
	if (!playerAction->
		CanUseAction(
			action))
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Selected action cannot be used.\n");

		// AP / MP不足時も通常状態へ戻す。
		m_PlayerInputMode =
			PlayerInputMode::None;

		return;
	}

	//=================================================
	// Definition
	//=================================================

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			action);

	if (!definition.Enabled)
	{
		OutputDebugStringA(
			"[FieldScene] "
			"Selected action is disabled.\n");

		m_PlayerInputMode =
			PlayerInputMode::None;

		return;
	}

	m_SelectedAction =
		action;

	//=================================================
	// Target Mode
	//=================================================

	switch (definition.TargetMode)
	{
		//=================================================
		// Self
		//=================================================

	case TargetingMode::Self:
	{
		// Self ActionはTarget選択不要。
		PlayerActionRequest request;

		request.Type =
			action;

		if (controller->
			RequestAction(
				request))
		{
			m_PlayerInputMode =
				PlayerInputMode::Executing;
		}
		else
		{
			// Skill未実装などで開始できなかった場合は
			// RadialMenuへ戻る。
			m_PlayerInputMode =
				PlayerInputMode::RadialMenu;
		}

		break;
	}

	//=================================================
	// Unit / Grid
	//=================================================
	case TargetingMode::Unit:
	case TargetingMode::Grid:
	{
		//=============================================
		// Target候補存在確認
		//=============================================
		//
		// 選択可能Targetが1件も無い場合は、
		// TargetSelectionへ入らず、
		// RadialMenuも閉じて通常状態へ戻す。
		//=============================================

		if (!HasSelectableTarget(
			action,
			definition))
		{
			OutputDebugStringA(
				"[FieldScene] "
				"No selectable target.\n");

			m_PlayerInputMode =
				PlayerInputMode::None;

			return;
		}

		m_PlayerInputMode =
			PlayerInputMode::TargetSelection;

		OutputDebugStringA(
			"[FieldScene] "
			"Entered TargetSelection.\n");

		break;
	}

	//=================================================
	// Direction
	//=================================================

	case TargetingMode::Direction:
	{
		// Direction Skillは
		// Skill実装時にEffectPatternを利用して
		// 選択可能方向を判定する。
		//
		// 現段階ではDirection選択へ進める。
		m_PlayerInputMode =
			PlayerInputMode::TargetSelection;

		break;
	}

	//=================================================
	// None
	//=================================================

	case TargetingMode::None:
	default:
	{
		// Item等はまだ未実装。
		m_PlayerInputMode =
			PlayerInputMode::RadialMenu;

		break;
	}
	}
}

void FieldScene::
CancelPlayerActionSelection()
{
	//=================================================
	// Radial操作終了
	//=================================================

	CloseRadialMenu();

	//=================================================
	// Action選択状態Reset
	//=================================================

	m_PlayerInputMode =
		PlayerInputMode::None;

	m_SelectedAction =
		PlayerActionType::Move;
}

//=====================================================
// Target Selection
//=====================================================
void FieldScene::UpdateTargetSelection()
{
	//=================================================
	// 左クリックされた瞬間のみ処理
	//=================================================

	if (!INPUT_MANAGER.IsMousePressed(
		MouseButton::Left))
	{
		return;
	}

	//=================================================
	// Selected Player
	//=================================================

	if (!m_SelectedPlayer ||
		m_SelectedPlayer->
		IsDestroy())
	{
		CancelPlayerActionSelection();

		return;
	}

	auto* controller =
		m_SelectedPlayer->
		GetComponent<
		PlayerControllerComponent>();

	auto* playerAction =
		m_SelectedPlayer->
		GetComponent<
		PlayerActionComponent>();

	auto* playerGrid =
		m_SelectedPlayer->
		GetComponent<
		GridPositionComponent>();

	if (!controller ||
		!playerAction ||
		!playerGrid)
	{
		CancelPlayerActionSelection();

		return;
	}

	//=================================================
	// Action Definition
	//=================================================

	const PlayerActionDefinition definition =
		playerAction->
		GetActionDefinition(
			m_SelectedAction);

	if (!definition.Enabled)
	{
		CancelPlayerActionSelection();

		return;
	}

	//=================================================
	// Mouse Grid
	//=================================================

	GridPosition clickedGrid
	{
		0,
		0
	};

	if (!TryGetMouseGrid(
		clickedGrid))
	{
		return;
	}

	//=================================================
	// Request
	//=================================================

	PlayerActionRequest request;

	request.Type =
		m_SelectedAction;

	bool actionStarted =
		false;

	//=================================================
	// Target Mode
	//=================================================

	switch (definition.TargetMode)
	{
		//=================================================
		// Grid
		//=================================================

	case TargetingMode::Grid:
	{
		request.TargetGrid =
			clickedGrid;

		actionStarted =
			controller->
			RequestAction(
				request);

		break;
	}

	//=================================================
	// Unit
	//=================================================

	case TargetingMode::Unit:
	{
		GameObject* target =
			m_GridMap.GetOccupant(
				clickedGrid);

		if (!target ||
			target->IsDestroy())
		{
			return;
		}

		request.TargetObject =
			target;

		actionStarted =
			controller->
			RequestAction(
				request);

		break;
	}

	//=================================================
	// Direction
	//=================================================

	case TargetingMode::Direction:
	{
		const GridPosition origin =
			playerGrid->
			GetGridPosition();

		GridDirection direction =
			GridDirection::North;

		if (!CalculateGridDirection(
			origin,
			clickedGrid,
			direction))
		{
			return;
		}

		request.Direction =
			direction;

		actionStarted =
			controller->
			RequestAction(
				request);

		break;
	}

	//=================================================
	// Self
	//=================================================
	//
	// SelfはBeginActionSelection()時点で
	// 即実行されるため、
	// 通常ここには来ない。
	//=================================================

	case TargetingMode::Self:
	{
		return;
	}

	case TargetingMode::None:
	default:
	{
		return;
	}
	}

	//=================================================
	// Action開始成功
	//=================================================

	if (actionStarted)
	{
		m_PlayerInputMode =
			PlayerInputMode::Executing;

		OutputDebugStringA(
			"[FieldScene] "
			"Action execution started.\n");

		return;
	}

	//=================================================
	// Action開始失敗
	//=================================================
	//
	// 無効なTargetをクリックしても、
	// TargetSelection自体は終了しない。
	//
	// 別のTargetを選び直せるようにする。
	//=================================================

	OutputDebugStringA(
		"[FieldScene] "
		"Invalid action target.\n");
}

//=====================================================
// Mouse Grid Picking
//=====================================================
bool FieldScene::TryGetMouseGrid(
	GridPosition& outGrid) const
{
	//=================================================
	// Camera
	//=================================================

	GameObject* cameraObject =
		m_gameObjectManager.FindByName(
			"MainCamera");

	if (!cameraObject)
	{
		return false;
	}

	auto* camera =
		cameraObject->GetComponent<
		CameraComponent>();

	if (!camera)
	{
		return false;
	}

	//=================================================
	// Mouse
	//=================================================

	const Vector2 mousePosition =
		INPUT_MANAGER.
		GetMousePosition();

	const Matrix4x4 view =
		camera->
		GetViewMatrix();

	const Matrix4x4 projection =
		camera->
		GetProjectionMatrix();

	Vector3 worldPosition(
		0.0f,
		0.0f,
		0.0f);

	//=================================================
	// Ground Picking
	//=================================================

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
		return false;
	}

	//=================================================
	// World → Grid
	//=================================================

	const GridPosition grid =
		m_GridMap.WorldToGrid(
			worldPosition);

	if (!m_GridMap.IsInside(
		grid))
	{
		return false;
	}

	outGrid =
		grid;

	return true;
}

//=====================================================
// Calculate Grid Direction
//=====================================================
bool FieldScene::CalculateGridDirection(
	const GridPosition& origin,
	const GridPosition& target,
	GridDirection& outDirection) const
{
	const int dx =
		target.X -
		origin.X;

	const int dy =
		target.Y -
		origin.Y;

	// 自分自身のCellでは
	// 方向を決定できない。
	if (dx == 0 &&
		dy == 0)
	{
		return false;
	}

	//=================================================
	// 大きく動いたAxisを優先
	//=================================================
	//
	// Grid:
	// X+ = East
	// X- = West
	// Y- = North
	// Y+ = South
	//=================================================

	if (std::abs(dx) >=
		std::abs(dy))
	{
		if (dx > 0)
		{
			outDirection =
				GridDirection::East;
		}
		else
		{
			outDirection =
				GridDirection::West;
		}
	}
	else
	{
		if (dy > 0)
		{
			outDirection =
				GridDirection::South;
		}
		else
		{
			outDirection =
				GridDirection::North;
		}
	}

	return true;
}

//=====================================================
// Has Selectable Target
//=====================================================

bool FieldScene::HasSelectableTarget(
	PlayerActionType action,
	const PlayerActionDefinition& definition) const
{
	if (!m_SelectedPlayer)
	{
		return false;
	}

	auto* gridPosition =
		m_SelectedPlayer->
		GetComponent<
		GridPositionComponent>();

	if (!gridPosition)
	{
		return false;
	}

	const GridPosition playerGrid =
		gridPosition->
		GetGridPosition();

	//=================================================
	// Move
	//=================================================
	//
	// MoveはTargetingSystemではなく、
	// CharacterStatusの移動距離と
	// GridRangeCalculatorを使用する。
	//=================================================

	if (action ==
		PlayerActionType::Move)
	{
		auto* status =
			m_SelectedPlayer->
			GetComponent<
			CharacterStatusComponent>();

		if (!status)
		{
			return false;
		}

		const int moveRangeDistance =
			status->
			CalculateMoveRange();

		if (moveRangeDistance <= 0)
		{
			return false;
		}

		const auto moveRange =
			GridRangeCalculator::
			CalculateMoveRange(
				m_GridMap,
				playerGrid,
				moveRangeDistance);

		return !moveRange.empty();
	}

	//=================================================
	// Unit / Grid Target
	//=================================================

	if (definition.TargetMode ==
		TargetingMode::Unit ||
		definition.TargetMode ==
		TargetingMode::Grid)
	{
		// TargetPatternが無効なら
		// TargetSelectionには入れない。
		if (!definition.TargetPattern.
			IsValid())
		{
			return false;
		}

		const auto targetableCells =
			TargetingSystem::
			GetTargetableCells(
				m_GridMap,
				m_SelectedPlayer,
				playerGrid,
				definition.TargetPattern,
				definition.Targeting);

		return !targetableCells.empty();
	}

	//=================================================
	// Self + Effect Pattern
	//=================================================
	//
	// Whirlwindなど、Self中心範囲Skill。
	// EffectPattern内に実際にEffectを適用できる対象が
	// 1体以上存在する場合のみ使用可能。
	//=================================================

	if (definition.TargetMode ==
		TargetingMode::Self &&
		definition.HasEffectPattern &&
		definition.EffectPattern.IsValid())
	{
		const auto offsets =
			definition.EffectPattern.
			GetOffsets(
				GridDirection::North);

		for (const GridOffset& offset :
			offsets)
		{
			const GridPosition targetPosition
			{
				playerGrid.X +
					offset.X,

				playerGrid.Y +
					offset.Y
			};

			if (!m_GridMap.IsInside(
				targetPosition))
			{
				continue;
			}

			const GameObject* target =
				m_GridMap.GetOccupant(
					targetPosition);

			if (!target ||
				target->IsDestroy())
			{
				continue;
			}

			// 現在のWarrior Ultimateは
			// Enemyのみ対象。
			if (target->GetTag() ==
				Tag::Enemy)
			{
				return true;
			}
		}

		return false;
	}

	//=================================================
	// Direction
	//=================================================
	//
	// DirectionはMouse方向を決めてから判定するため、
	// この段階では許可する。
	//=================================================

	if (definition.TargetMode ==
		TargetingMode::Direction)
	{
		return true;
	}

	return true;
}