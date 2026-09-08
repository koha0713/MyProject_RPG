#pragma once

#include <d3d11.h>
#include <cstdint>

#include "CommonType.h"
#include "NonCopyable.h"

/**
 * @brief 頂点バッファとインデックスバッファを管理するクラス
 *
 * @details
 * CPU上の頂点・インデックスデータからDirectX11のGPUバッファを生成し、
 * 描画時のバインドとDrawIndexedを担当する。
 *
 * MeshBuffer自身はモデルファイルの読み込み方法を知らず、
 * GPUバッファ管理だけを責務とする。
 */
class MeshBuffer : private NonCopyable
{
public:

	MeshBuffer() = default;
	~MeshBuffer() = default;

	//====================
	// GPUバッファ生成
	//====================

	/**
	 * @brief 頂点・インデックスバッファを作成
	 *
	 * @param device D3D11Device
	 * @param vertexData 頂点データ先頭アドレス
	 * @param vertexStride 1頂点あたりのサイズ
	 * @param vertexCount 頂点数
	 * @param indexData インデックスデータ
	 * @param indexCount インデックス数
	 *
	 * @return 作成成功ならtrue
	 */
	bool Create(
		ID3D11Device* device,
		const void* vertexData,
		uint32_t vertexStride,
		uint32_t vertexCount,
		const uint32_t* indexData,
		uint32_t indexCount);

	/**
	 * @brief GPUバッファを解放
	 */
	void Release();

	//====================
	// 描画
	//====================

	/**
	 * @brief MeshBufferをInputAssemblerへ設定
	 */
	void Bind(ID3D11DeviceContext* context) const;

	/**
	 * @brief インデックス描画
	 */
	void Draw(ID3D11DeviceContext* context) const;

	//====================
	// Getter
	//====================

	uint32_t GetVertexCount() const
	{
		return m_VertexCount;
	}

	uint32_t GetIndexCount() const
	{
		return m_IndexCount;
	}

	uint32_t GetVertexStride() const
	{
		return m_VertexStride;
	}

	bool IsValid() const
	{
		return m_VertexBuffer != nullptr &&
			m_IndexBuffer != nullptr;
	}

private:

	//====================
	// GPUリソース
	//====================

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;

	//====================
	// Buffer情報
	//====================

	uint32_t m_VertexStride = 0;
	uint32_t m_VertexCount = 0;
	uint32_t m_IndexCount = 0;
};