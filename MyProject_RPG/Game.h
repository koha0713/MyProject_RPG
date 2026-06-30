#pragma once

#include <memory>

#include "Window.h"


//==============================
// ゲームクラス
//==============================
class Game : private NonCopyable
{
public:
	//====================
	// コンストラクタ・デストラクタ
	//====================
	/**
	 * @brief コンストラクタ
	 */
	Game() = default;

	/**
	 * @brief デストラクタ
	 */
	~Game() = default;

	//====================
	// ゲーム実行
	//====================

	/**
	 * @brief ゲーム実行
	 * @details ゲームのメインループを実行する関数
	 */
	int Run();

protected:
	//====================
	// ライフサイクル
	//====================
	/**
	 * @brief 初期化
	 * @details ゲームの初期化処理を行う関数
	 */
	virtual bool Initialize();

	/**
	 * @brief 終了
	 * @details ゲームの終了処理を行う関数
	 */
	virtual void Finalize();

	/**
	 * @brief 更新
	 * @details ゲームの更新処理を行う関数
	 */
	virtual void Update();

	/**
	 * @brief 描画
	 * @details ゲームの描画処理を行う関数
	 */
	virtual void Draw();

protected:
	//====================
	// メンバ変数
	//====================
	std::unique_ptr<Window> m_window;	//ウィンドウクラス

};

