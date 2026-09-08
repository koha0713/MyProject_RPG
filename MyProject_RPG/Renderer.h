#pragma once

/**
 * @file Renderer.h
 * @brief DirectX11の描画基盤を管理するRendererクラス
 */

#include <d3d11.h>
#include <dxgi.h>

#include "CommonType.h"
#include "NonCopyable.h"


 // DirectX11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class MeshBuffer;
struct MaterialData;

/**
 * @brief ブレンド方法
 */
enum class BlendState
{
	None = 0,
	Alpha,
	Additive,

	Count
};

/**
 * @brief Renderer
 *
 * @details
 * DirectX11のDevice / DeviceContext / SwapChainなどの
 * グラフィックス基盤を管理する。
 *
 * ModelDataなどのゲーム固有リソースそのものは所有せず、
 * 描画に必要なGPU状態と描画処理のみを担当する。
 */
class Renderer : private NonCopyable
{
public:

	//====================
	// ライフサイクル
	//====================

	/**
	 * @brief Renderer初期化
	 *
	 * @return 初期化成功ならtrue
	 */
	static bool Init();

	/**
	 * @brief Renderer終了
	 */
	static void Dispose();

	//====================
	// Frame
	//====================

	/**
	 * @brief フレーム描画開始
	 */
	static void Begin();

	/**
	 * @brief フレーム描画終了
	 */
	static void End();

	//====================
	// Model描画
	//====================

	/**
	 * @brief Meshを描画
	 *
	 * @param mesh 描画するMeshBuffer
	 * @param material 使用するマテリアル
	 * @param worldMatrix ワールド行列
	 */
	static void DrawMesh(
		const MeshBuffer& mesh,
		const MaterialData& material,
		const Matrix4x4& worldMatrix);

	//====================
	// RenderState
	//====================

	/**
	 * @brief DepthTest切り替え
	 */
	static void SetDepthEnable(bool enable);

	/**
	 * @brief BlendState変更
	 */
	static void SetBlendState(BlendState state);

	/**
	 * @brief カリング設定
	 *
	 * @param enable trueならBackFaceCulling
	 */
	static void SetCulling(bool enable);

	/**
	 * @brief FillMode変更
	 */
	static void SetFillMode(
		D3D11_FILL_MODE fillMode);

	//====================
	// Getter
	//====================

	static ID3D11Device* GetDevice()
	{
		return m_Device.Get();
	}

	static ID3D11DeviceContext* GetDeviceContext()
	{
		return m_DeviceContext.Get();
	}

	static IDXGISwapChain* GetSwapChain()
	{
		return m_SwapChain.Get();
	}

private:

	//====================
	// 初期化処理
	//====================

	/**
	 * @brief Device / SwapChain生成
	 */
	static bool CreateDevice();

	/**
	 * @brief RenderTarget生成
	 */
	static bool CreateRenderTarget();

	/**
	 * @brief DepthBuffer生成
	 */
	static bool CreateDepthStencil();

	/**
	 * @brief Viewport設定
	 */
	static void CreateViewport();

	/**
	 * @brief RenderState生成
	 */
	static bool CreateRenderStates();

	/**
	 * @brief モデル描画Pipeline生成
	 */
	static bool CreateModelPipeline();

private:

	//====================
	// DirectX基本オブジェクト
	//====================

	static D3D_FEATURE_LEVEL m_FeatureLevel;

	static ComPtr<ID3D11Device>
		m_Device;

	static ComPtr<ID3D11DeviceContext>
		m_DeviceContext;

	static ComPtr<IDXGISwapChain>
		m_SwapChain;

	static ComPtr<ID3D11RenderTargetView>
		m_RenderTargetView;

	static ComPtr<ID3D11DepthStencilView>
		m_DepthStencilView;

	//====================
	// RenderState
	//====================

	static ComPtr<ID3D11DepthStencilState>
		m_DepthStateEnable;

	static ComPtr<ID3D11DepthStencilState>
		m_DepthStateDisable;

	static ComPtr<ID3D11BlendState>
		m_BlendStates[
			static_cast<size_t>(
				BlendState::Count)];

	static ComPtr<ID3D11RasterizerState>
		m_RasterizerSolidCull;

	static ComPtr<ID3D11RasterizerState>
		m_RasterizerSolidNoCull;

	static ComPtr<ID3D11RasterizerState>
		m_RasterizerWireframeCull;

	static ComPtr<ID3D11RasterizerState>
		m_RasterizerWireframeNoCull;

	// 現在のRasterizer設定
	static bool m_CullingEnabled;

	static D3D11_FILL_MODE
		m_CurrentFillMode;

	//====================
	// Model Pipeline
	//====================

	static ComPtr<ID3D11VertexShader>
		m_ModelVertexShader;

	static ComPtr<ID3D11PixelShader>
		m_ModelPixelShader;

	static ComPtr<ID3D11InputLayout>
		m_ModelInputLayout;

	static ComPtr<ID3D11Buffer>
		m_ModelConstantBuffer;
};