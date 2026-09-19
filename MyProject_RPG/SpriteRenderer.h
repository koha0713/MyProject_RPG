#pragma once

/**
 * @file SpriteRenderer.h
 * @brief DirectX11‚É‚æ‚éScreen‹óŠÔSprite•`‰æ
 */

#include <d3d11.h>

#include "CommonType.h"
#include "NonCopyable.h"

 /**
  * @brief Screen‹óŠÔ‚Ì2D‰æ‘œ•`‰æ
  */
class SpriteRenderer :
	private NonCopyable
{
public:

	//====================
	// Lifecycle
	//====================

	static bool Initialize();

	static void Finalize();

	//====================
	// Frame
	//====================

	/**
	 * @brief UI•`‰æŠJn
	 */
	static void Begin();

	/**
	 * @brief UI•`‰æI—¹
	 */
	static void End();

	//====================
	// Draw
	//====================

	/**
	 * @brief Texture‚ğScreenÀ•W‚Ö•`‰æ
	 *
	 * @param texture •`‰æTexture
	 * @param position ¶ãÀ•W
	 * @param size •`‰æƒTƒCƒY
	 * @param color æZColor
	 */
	static void Draw(
		ID3D11ShaderResourceView* texture,
		const Vector2& position,
		const Vector2& size,
		const Color& color);

private:

	static bool CreatePipeline();

private:

	static ComPtr<ID3D11VertexShader>
		m_VertexShader;

	static ComPtr<ID3D11PixelShader>
		m_PixelShader;

	static ComPtr<ID3D11InputLayout>
		m_InputLayout;

	static ComPtr<ID3D11Buffer>
		m_VertexBuffer;

	static ComPtr<ID3D11SamplerState>
		m_SamplerState;

	static bool
		m_IsDrawing;
};