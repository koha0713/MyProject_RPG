#pragma once

#include <d3d11.h>
#include <string>

#include "CommonType.h"
#include "NonCopyable.h"

/**
 * @brief 2Dテクスチャを管理するクラス
 *
 * @details
 * stb_imageで画像をCPUメモリへ読み込み、
 * DirectX11のTexture2D / ShaderResourceViewを生成する。
 *
 * Texture自身はキャッシュ管理を行わず、
 * 同一Textureの共有はResourceManagerが担当する。
 */
class Texture : private NonCopyable
{
public:

	Texture() = default;
	~Texture() = default;

	//====================
	// ライフサイクル
	//====================

	/**
	 * @brief 画像ファイルからTextureを生成
	 *
	 * @param device DirectX11 Device
	 * @param filePath 画像ファイルパス
	 *
	 * @return 成功した場合true
	 */
	bool Load(
		ID3D11Device* device,
		const std::string& filePath);

	/**
	 * @brief GPUリソースを解放
	 */
	void Release();

	//====================
	// Getter
	//====================

	/**
	 * @brief ShaderResourceView取得
	 */
	ID3D11ShaderResourceView*
		GetShaderResourceView() const
	{
		return m_ShaderResourceView.Get();
	}

	/**
	 * @brief Texture2D取得
	 */
	ID3D11Texture2D*
		GetTexture() const
	{
		return m_Texture.Get();
	}

	uint32_t GetWidth() const
	{
		return m_Width;
	}

	uint32_t GetHeight() const
	{
		return m_Height;
	}

	bool IsValid() const
	{
		return m_Texture != nullptr &&
			m_ShaderResourceView != nullptr;
	}

private:

	//====================
	// GPUリソース
	//====================

	ComPtr<ID3D11Texture2D>
		m_Texture;

	ComPtr<ID3D11ShaderResourceView>
		m_ShaderResourceView;

	//====================
	// Texture情報
	//====================

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
};