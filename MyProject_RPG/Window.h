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

	//====================
	// メッセージ更新
	//====================
	bool ProcessMessage();

	//====================
	// Getter関数
	//====================
	static HWND GetHandle() { return m_hWnd; }
	static HINSTANCE GetInstanceHandl() { return m_hInstance; }
	static uint32_t GetWidth() { return m_width; }
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
	static HWND m_hWnd;	// ウィンドウハンドル
	static HINSTANCE m_hInstance;	// インスタンスハンドル
	static uint32_t m_width;
	static uint32_t m_height;

};