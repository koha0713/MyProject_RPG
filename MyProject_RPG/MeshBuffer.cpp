#include "MeshBuffer.h"

#include <limits>

/**
 * @brief GPUバッファを作成
 */
bool MeshBuffer::Create(
	ID3D11Device* device,
	const void* vertexData,
	uint32_t vertexStride,
	uint32_t vertexCount,
	const uint32_t* indexData,
	uint32_t indexCount)
{
	//====================
	// 引数チェック
	//====================

	if (!device ||
		!vertexData ||
		!indexData ||
		vertexStride == 0 ||
		vertexCount == 0 ||
		indexCount == 0)
	{
		return false;
	}

	// ByteWidthはUINTなのでオーバーフローをチェックする
	const uint64_t vertexBufferSize =
		static_cast<uint64_t>(vertexStride) *
		static_cast<uint64_t>(vertexCount);

	const uint64_t indexBufferSize =
		sizeof(uint32_t) *
		static_cast<uint64_t>(indexCount);

	if (vertexBufferSize > (std::numeric_limits<UINT>::max)() ||
		indexBufferSize > (std::numeric_limits<UINT>::max)())
	{
		return false;
	}

	// 以前のバッファが存在する場合は破棄
	Release();

	//====================
	// VertexBuffer作成
	//====================

	D3D11_BUFFER_DESC vertexBufferDesc{};

	vertexBufferDesc.ByteWidth =
		static_cast<UINT>(vertexBufferSize);

	vertexBufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	vertexBufferDesc.BindFlags =
		D3D11_BIND_VERTEX_BUFFER;

	vertexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexInitialData{};

	vertexInitialData.pSysMem =
		vertexData;

	HRESULT hr =
		device->CreateBuffer(
			&vertexBufferDesc,
			&vertexInitialData,
			m_VertexBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		Release();
		return false;
	}

	//====================
	// IndexBuffer作成
	//====================

	D3D11_BUFFER_DESC indexBufferDesc{};

	indexBufferDesc.ByteWidth =
		static_cast<UINT>(indexBufferSize);

	indexBufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	indexBufferDesc.BindFlags =
		D3D11_BIND_INDEX_BUFFER;

	indexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA indexInitialData{};

	indexInitialData.pSysMem =
		indexData;

	hr =
		device->CreateBuffer(
			&indexBufferDesc,
			&indexInitialData,
			m_IndexBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		Release();
		return false;
	}

	//====================
	// Mesh情報保存
	//====================

	m_VertexStride = vertexStride;
	m_VertexCount = vertexCount;
	m_IndexCount = indexCount;

	return true;
}

/**
 * @brief GPUリソース解放
 */
void MeshBuffer::Release()
{
	m_VertexBuffer.Reset();
	m_IndexBuffer.Reset();

	m_VertexStride = 0;
	m_VertexCount = 0;
	m_IndexCount = 0;
}

/**
 * @brief InputAssemblerへ設定
 */
void MeshBuffer::Bind(
	ID3D11DeviceContext* context) const
{
	if (!context || !IsValid())
	{
		return;
	}

	const UINT stride =
		m_VertexStride;

	const UINT offset = 0;

	ID3D11Buffer* vertexBuffer =
		m_VertexBuffer.Get();

	context->IASetVertexBuffers(
		0,
		1,
		&vertexBuffer,
		&stride,
		&offset);

	context->IASetIndexBuffer(
		m_IndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0);

	// Assimp側でTriangulateしているため三角形リスト
	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/**
 * @brief DrawIndexed
 */
void MeshBuffer::Draw(
	ID3D11DeviceContext* context) const
{
	if (!context || !IsValid())
	{
		return;
	}

	context->DrawIndexed(
		m_IndexCount,
		0,
		0);
}