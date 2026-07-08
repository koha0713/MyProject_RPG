#pragma once

#include <d3d11.h>
#include <vector>
#include <functional>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class DebugUI
{
	//====================
	// メンバ変数
	//====================
	/**
	 * @brief デバッグUI関数のリスト
	 */
	static std::vector<std::function<void()>> m_debugUIFunctions;

public:
	/**
	 * @brief デバッグUIの初期化
	 * @param device Direct3D11デバイス
	 * @param context Direct3D11デバイスコンテキスト
	 */
	static void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);

	/**
	 * @brief デバッグUIの終了処理
	 */
	static void Finalize();

	 /**
	  * @brief デバッグUIの描画
	  */
	static void Render();

	/**
	 * @brief デバッグUI関数を登録
	 * @param func 登録するデバッグUI関数
	 */
	static void RegisterDebugFunction(const std::function<void()>& func);

};
