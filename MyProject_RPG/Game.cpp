#include "Game.h"
#include "Fpscontrol.h"

#include "Renderer.h"
#include "DebugUI.h"
#include "SceneManager.h"

//==============================
// ゲーム実行関数
//==============================
int Game::Run()
{
	//=================
	// ゲーム初期化
	//=================
	if (!Initialize())
	{
		return -1;
	}

	//=================
	// ゲームループ
	//=================
	while (true)
	{
		// メッセージ処理
		if (!m_window->ProcessMessage())
		{
			break;
		}

		// フレームの待ち時間を計算
		uint64_t delta_time = 0;
		static FPS fpsrate(120);
		
		// 前回実行からの経過時間を計算
		delta_time = fpsrate.BeginFrame();

		// ゲーム更新
		Update(delta_time);

		// ゲーム描画
		Draw(delta_time);

		// 規定時間まで待つ
		fpsrate.EndFrame();
	}

	//=================
	// ゲーム終了
	//=================
	Finalize();

	return 0;
}

//==============================
// ゲーム初期化
//==============================
bool Game::Initialize()
{
	//=================
	// ウィンドウ生成
	//=================
	m_window = std::make_unique<Window>();

	if (!m_window->Create(1280,720))
	{
		return false;
	}

	//====================
	// Renderer初期化
	//====================

	if (!Renderer::Init())
	{
		return false;
	}

	//=================
	// デバッグUIの初期化
	//=================
	DebugUI::Initialize(
		Renderer::GetDevice(),
		Renderer::GetDeviceContext());


	SceneManager::Initialize();
	SceneManager::SetCurrentScene("TestScene");

	return true;
}

//==============================
// ゲーム終了
//==============================
void Game::Finalize()
{
	//====================
	// Scene
	//====================

	SceneManager::Finalize();

	//====================
	// DebugUI
	//====================

	DebugUI::Finalize();

	//====================
	// Renderer
	//====================

	Renderer::Dispose();

	//====================
	// Window
	//====================

	if (m_window)
	{
		m_window->Finalize();
		m_window.reset();
	}
}

//==============================
// ゲーム更新
//==============================
void Game::Update(uint64_t delta)
{
	SceneManager::Update(delta);
}

//==============================
// ゲーム描画
//==============================
void Game::Draw(uint64_t delta)
{
	Renderer::Begin();

	DebugUI::BeginFrame();
	SceneManager::Draw(delta);
	DebugUI::EndFrame();
	
	Renderer::End();
}