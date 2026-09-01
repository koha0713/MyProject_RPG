#pragma once

#include "Windows.h"
#include <string>
#include "NonCopyable.h"

//=========================
// ウィンドウクラス
//=========================
class Window : public NonCopyable
{
public:
	//====================
	// ライフサイクル
	//====================
	/**
	 * @brief コンストラクタ
	 */
	Window() = default;

	/**
	 * @brief デストラクタ
	 */
	~Window() = default;

	//====================
	// ウィンドウ作成
	//====================
	/**
	 * @brief ウィンドウ作成関数
	 * @param title ウィンドウ名
	 * @param width/height ウィンドウサイズ
	 */
	bool Create(
		int width,
		int height);

	void Finalize();

	//====================
	// メッセージ更新
	//====================
	bool ProcessMessage();

	//====================
	// Getter関数
	//====================
	/**
	 * @brief ウィンドウハンドルを取得する
	 * @return ウィンドウハンドル
	 */
	static HWND GetWindow() { return m_hWnd; }
	/**
	 * @brief インスタンスハンドルを取得する
	 * @return インスタンスハンドル
	 */
	static HINSTANCE GetHInstance() { return m_hInstance; }
	/**
	 * @brief ウィンドウの横幅を取得する
	 * @return ウィンドウの横幅
	 */
	static uint32_t GetWidth() { return m_width; }
	/**
	 * @brief ウィンドウの縦幅を取得する
	 * @return ウィンドウの縦幅
	 */
	static uint32_t GetHeight() { return m_height; }

private:
	//====================
	// ウィンドウプロシージャ
	//====================
	/**
	 * @brief ウィンドウプロシージャ
	 * @details ウィンドウに送られるメッセージを処理する関数
	 * @param hWnd ウィンドウへのハンドル
	 * @param uMsg メッセージコード
	 * @param wParam/lParam メッセージ関連の追加データ
	 * @return 処理結果
	 */
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

private:
	//====================
	// メンバ変数
	//====================
	static HWND m_hWnd;				// ウィンドウハンドル
	static HINSTANCE m_hInstance;	// インスタンスハンドル
	static uint32_t m_width;		// ウィンドウの横幅
	static uint32_t m_height;		// ウィンドウの縦幅

};