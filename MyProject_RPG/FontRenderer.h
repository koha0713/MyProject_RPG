#pragma once

/**
 * @file FontRenderer.h
 * @brief DirectWrite / Direct2Dを使用したFont描画
 */

#include <string>

#include <d2d1_1.h>
#include <d2d1_1helper.h>

#include <dwrite.h>
#include <dwrite_3.h>

#include <wrl/client.h>

#include "CommonType.h"

 //=====================================================
 // Library
 //=====================================================

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

/**
 * @brief Font描画管理クラス
 *
 * @details
 * DirectWriteでFontを管理し、
 * Direct2Dを使用してDirectX11のBackBufferへ
 * 文字列を描画する。
 *
 * ゲーム内では1種類のFontファイルのみを使用し、
 * FontRendererがそのFontResourceを共有管理する。
 */
class FontRenderer
{
public:

	//=================================================
	// Lifecycle
	//=================================================

	/**
	 * @brief FontRenderer初期化
	 *
	 * @param fontPath
	 * 使用するFontファイルへのパス
	 *
	 * @return 初期化成功時true
	 */
	static bool Initialize(
		const std::wstring& fontPath);

	/**
	 * @brief FontRenderer終了
	 */
	static void Finalize();

	//=================================================
	// Frame
	//=================================================

	/**
	 * @brief Font描画開始
	 */
	static void Begin();

	/**
	 * @brief Font描画終了
	 */
	static void End();

	//=================================================
	// Draw
	//=================================================

	/**
	 * @brief 文字列を描画
	 *
	 * @param text 描画文字列
	 * @param fontSize Fontサイズ
	 * @param position 描画位置
	 * @param size 描画領域
	 * @param color 文字色
	 */
	static void DrawText(
		const std::wstring& text,
		float fontSize,
		const Vector2& position,
		const Vector2& size,
		const Color& color);

private:

	//=================================================
	// Custom Font
	//=================================================

	/**
	 * @brief Fontファイルから
	 *        Custom Font Collectionを生成する
	 */
	static bool CreateCustomFont(
		const std::wstring& fontPath);

	/**
	 * @brief Custom Font Collectionから
	 *        Font Family名を取得する
	 */
	static bool AcquireFontFamilyName();

private:

	//=================================================
	// Direct2D
	//=================================================

	static Microsoft::WRL::ComPtr<
		ID2D1Factory1>
		m_D2DFactory;

	static Microsoft::WRL::ComPtr<
		ID2D1Device>
		m_D2DDevice;

	static Microsoft::WRL::ComPtr<
		ID2D1DeviceContext>
		m_D2DContext;

	static Microsoft::WRL::ComPtr<
		ID2D1Bitmap1>
		m_TargetBitmap;

	static Microsoft::WRL::ComPtr<
		ID2D1SolidColorBrush>
		m_Brush;

	//=================================================
	// DirectWrite
	//=================================================

	/**
	 * @brief 基本DirectWrite Factory
	 */
	static Microsoft::WRL::ComPtr<
		IDWriteFactory>
		m_DWriteFactory;

	/**
	 * @brief Custom Font関連API用Factory
	 */
	static Microsoft::WRL::ComPtr<
		IDWriteFactory5>
		m_DWriteFactory5;

	/**
	 * @brief Custom Font Collection
	 */
	static Microsoft::WRL::ComPtr<
		IDWriteFontCollection1>
		m_FontCollection;

	/**
	 * @brief Font Family名
	 *
	 * CreateTextFormatで指定する。
	 */
	static std::wstring
		m_FontFamilyName;

	//=================================================
	// State
	//=================================================

	static bool
		m_IsDrawing;
};