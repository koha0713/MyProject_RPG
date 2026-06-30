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
	HWND GetHandle() const { return m_hWnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

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
	HWND m_hWnd = nullptr;	// ウィンドウハンドル
	HINSTANCE m_hInstance = nullptr;	// インスタンスハンドル
	int m_width = 1920;
	int m_height = 1080;

};