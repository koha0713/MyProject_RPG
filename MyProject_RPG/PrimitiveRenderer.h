#pragma once

/**
 * @file PrimitiveRenderer.h
 * @brief Lineなどの単純Primitiveを描画するRenderer
 */

#include <d3d11.h>
#include <vector>

#include "CommonType.h"
#include "NonCopyable.h"

 /**
  * @brief Line描画用頂点
  */
struct LineVertex
{
	Vector3 Position =
		Vector3(
			0.0f,
			0.0f,
			0.0f);

	Color VertexColor =
		Color(
			1.0f,
			1.0f,
			1.0f,
			1.0f);
};

/**
 * @brief 単純Primitive描画
 *
 * @details
 * Grid、Ray、経路、デバッグ形状など、
 * 軽量なLine描画を担当する。
 *
 * DirectX Device / Contextは所有せず、
 * Rendererから取得して使用する。
 */
class PrimitiveRenderer :
	private NonCopyable
{
public:

	//====================
	// Lifecycle
	//====================

	static bool Initialize();

	static void Finalize();

	//====================
	// Line
	//====================

	/**
	 * @brief Lineを1本描画
	 */
	static void DrawLine(
		const Vector3& start,
		const Vector3& end,
		const Color& color);

	/**
	 * @brief Lineを一括描画
	 *
	 * @details
	 * 2頂点で1Line。
	 *
	 * vertices[0] - vertices[1]
	 * vertices[2] - vertices[3]
	 * ...
	 */
	static void DrawLines(
		const std::vector<LineVertex>& vertices);

	/**
	 * @brief 三角形群を描画する
	 *
	 * @details
	 * 3頂点で1Triangleとして扱う。
	 */
	static void DrawTriangles(
		const std::vector<LineVertex>& vertices);

private:

	static bool CreatePipeline();

	/**
	 * @brief 必要頂点数に応じてDynamicVertexBufferを確保する
	 */
	static bool EnsureVertexBuffer(
		size_t vertexCount);

private:

	static ComPtr<ID3D11VertexShader>
		m_VertexShader;

	static ComPtr<ID3D11PixelShader>
		m_PixelShader;

	static ComPtr<ID3D11InputLayout>
		m_InputLayout;

	static ComPtr<ID3D11Buffer>
		m_VertexBuffer;

	static ComPtr<ID3D11Buffer>
		m_ConstantBuffer;

	/**
	 * @brief 現在確保済みの頂点数
	 */
	static size_t m_VertexCapacity;
};