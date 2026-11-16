#pragma once

#include <cstdint>
#include <vector>

#include <d3d11.h>
#include <wrl/client.h>

#include "VisibilityMap.h"

/**
 * @brief VisibilityMapをGPUへ渡すためのTexture
 *
 * @details
 * 1Pixel = 1Gridとして扱う。
 *
 * R値:
 *
 * 0.0 = Unexplored
 * 0.5 = Explored
 * 1.0 = Visible
 */
class VisibilityTexture
{
public:

	VisibilityTexture() = default;

	~VisibilityTexture() = default;

	/**
	 * @brief GPU Textureを生成
	 */
	bool Initialize(
		ID3D11Device* device,
		int width,
		int height);

	/**
	 * @brief VisibilityMapからTexture内容を更新
	 */
	bool Update(
		ID3D11DeviceContext* context,
		const VisibilityMap& visibilityMap);

	/**
	 * @brief Resource解放
	 */
	void Finalize();

	/**
	 * @brief Shader Resource View取得
	 */
	ID3D11ShaderResourceView*
		GetShaderResourceView() const
	{
		return
			m_ShaderResourceView.Get();
	}

	bool IsValid() const
	{
		return
			m_Texture != nullptr &&
			m_ShaderResourceView != nullptr;
	}

	int GetWidth() const
	{
		return m_Width;
	}

	int GetHeight() const
	{
		return m_Height;
	}

private:

	int m_Width =
		0;

	int m_Height =
		0;

	/**
	 * @brief GPU Visibility Texture
	 */
	Microsoft::WRL::ComPtr<
		ID3D11Texture2D>
		m_Texture;

	/**
	 * @brief Shaderから読むためのSRV
	 */
	Microsoft::WRL::ComPtr<
		ID3D11ShaderResourceView>
		m_ShaderResourceView;

	/**
	 * @brief Texture更新用CPU Buffer
	 *
	 * DXGI_FORMAT_R8_UNORMなので
	 * 1Pixel = uint8_t。
	 */
	std::vector<uint8_t>
		m_UploadBuffer;
};