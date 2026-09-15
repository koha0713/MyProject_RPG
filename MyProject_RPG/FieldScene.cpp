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

#include <ModelLoader.h>

#include "InputManager.h"
#include "SoundManager.h"

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
		}
		{
			auto* ranger =
				m_gameObjectManager.Create<GameObject>();

			if (!ranger)
			{
				return;
			}

			ranger->SetName("Ranger");
			ranger->SetTag(Tag::Player);

			//====================
			// TransformComponent
			//====================
			{
				auto* transform =
					ranger->AddComponent<TransformComponent>();

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
					ranger->AddComponent<ModelComponent>();

				if (model)
				{
					// 使用するモデルの実際のパスに変更する
					const bool result =
						model->SetModel(
							"Assets/Models/Player/Ranger.fbx");
					// Material[0]へTextureを手動設定
					const bool texture0Result =
						model->SetTexture(
							0,
							"Assets/Models/Player/Ranger_Texture.png");
					// Material[1]へTextureを手動設定
					const bool texture1Result =
						model->SetTexture(
							1,
							"Assets/Models/Player/Ranger_Bow_Texture.png");

					// 初期Animation設定
					model->PlayAnimation(AnimationID::Idle, true, 0.0f);
				}
			}
			//====================
			// GridPosition
			//====================
			auto* gridPosition =
				ranger->AddComponent<
				GridPositionComponent>();
			gridPosition->SetGridMap(&m_GridMap);
			gridPosition->SetGridPosition(
				GridPosition
				{
					4,
					2
				});
		}
		{
			auto* monk =
				m_gameObjectManager.Create<GameObject>();

			if (!monk)
			{
				return;
			}

			monk->SetName("Monk");
			monk->SetTag(Tag::Player);

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
			rogue->SetTag(Tag::Player);

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
			wizard->SetTag(Tag::Player);

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
					0,
					0
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

	m_TurnManager.Initialize();
	m_gameObjectManager.Initialize();

}

void FieldScene::Finalize()
{
	// GameObjectの終了処理
	m_gameObjectManager.Clear();
}

void FieldScene::Update(uint64_t delta)
{
	if (m_TurnManager.IsEnemyTurn())
	{
		OutputDebugStringA(
			"[Turn] Enemy Turn\n");

		// 仮処理
		m_TurnManager.EndEnemyTurn();

		OutputDebugStringA(
			"[Turn] Player Turn\n");
	}
	// GameObjectの更新
	m_gameObjectManager.Update(delta);


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
			m_gameObjectManager.DrawDebugUI();
		});

}