/**
 * @file Renderer.cpp
 * @brief DirectX11 Renderer実装
 */

#include "Renderer.h"

#include <cstddef>
#include <cmath>
#include <d3dcompiler.h>

#include "Window.h"
#include "MeshBuffer.h"
#include "ModelData.h"

#include "WaterRenderSettings.h"
#include "Texture.h"
#include "WaterRenderData.h"
#include "GrassRenderData.h"
#include "TerrainVertex.h"
#include "VisibilityTexture.h"

 //=====================================================
 // 内部定義
 //=====================================================

namespace
{
	/**
	 * @brief Model描画用ConstantBuffer
	 *
	 * @details
	 * HLSL側のModelConstantBufferと
	 * メモリレイアウトを一致させること。
	 */
	struct ModelConstantBuffer
	{
		/**
		 * @brief World Matrix
		 */
		Matrix4x4 World;
		/**
		 * @brief World * View * Projection
		 */
		Matrix4x4 WorldViewProjection;

		/**
		 * @brief 法線変換用World逆転置行列
		 */
		Matrix4x4 WorldInverseTranspose;

		/**
		 * @brief MaterialのDiffuse Color
		 */
		Vector4 DiffuseColor;

		/**
		 * @brief xyz : 光が進む方向
		 *        w   : 光強度
		 */
		Vector4 LightDirectionIntensity;

		/**
		 * @brief Directional Light色
		 */
		Vector4 LightColor;

		/**
		 * @brief Ambient Light色
		 */
		Vector4 AmbientLightColor;

		/**
		 * @brief Toon Shading
		 * x : HighlightThreshold
		 * y : ShadowThreshold
		 * z : MidToneIntensity
		 * w : ShadowIntensity
		 */
		Vector4 ToonParameters;

		/**
		 * @brief Textureを使用するか
		 */
		uint32_t HasTexture = 0;

		/**
		 * @brief ConstantBuffer 16byte Alignment
		 */
		float Padding[3]{};
	};

	static_assert(
		sizeof(ModelConstantBuffer) % 16 == 0,
		"ModelConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief Water Shader用ConstantBuffer
	 *
	 * @note HLSL側のWaterCommon.hlsliと
	 *       メンバ順を完全に一致させること。
	 */
	struct WaterConstantBuffer
	{
		Matrix4x4 World;

		Matrix4x4 WorldViewProjection;

		// 水面基本色
		Vector4 WaterColor;

		// xy = FlowDirection
		// z  = FlowSpeed
		// w  = Tiling
		Vector4 FlowA;

		Vector4 FlowB;

		// x = NormalStrength
		// y = Time
		// z = HasNormalMapA
		// w = HasNormalMapB
		Vector4 NormalParameters;

		// xyz = Direction
		// w   = Intensity
		Vector4 LightDirectionIntensity;

		Vector4 LightColor;

		Vector4 AmbientLightColor;

		// xyz = Camera World Position
		// w   = unused
		Vector4 CameraPosition;

		// x = SpecularStrength
		// y = SpecularPower
		// z = FresnelStrength
		// w = FresnelPower
		Vector4 ReflectionParameters;

		Vector4 FresnelColor;

		// RGB = Shallow Color
		Vector4 ShallowWaterColor;

		// RGB = Deep Color
		Vector4 DeepWaterColor;

		// x = ShallowDepth
		// y = DeepDepth
		// z = ShallowAlpha
		// w = DeepAlpha
		Vector4 DepthParameters;
	};

	static_assert(
		sizeof(WaterConstantBuffer) % 16 == 0,
		"WaterConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief Terrain Shader用ConstantBuffer
	 */
	struct TerrainConstantBuffer
	{
		Matrix4x4 World;

		Matrix4x4 WorldViewProjection;

		// x = Tiling
		// yzw = unused
		Vector4 TerrainParameters;

		Vector4 TerrainColor;

		Vector4 LightDirectionIntensity;

		Vector4 LightColor;

		Vector4 AmbientLightColor;

		// Toon設定
		Vector4 ToonParameters;
	};

	static_assert(
		sizeof(TerrainConstantBuffer) % 16 == 0,
		"TerrainConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief Grass Shader用ConstantBuffer
	 */
	struct GrassConstantBuffer
	{
		Matrix4x4 ViewProjection;

		// x = Time
		// y = WindStrength
		// z = WindSpeed
		// w = AlphaClip
		Vector4 GrassParameters;

		// xy = WindDirection
		Vector4 WindParameters;

		Vector4 VisibilityParameters;
	};

	static_assert(
		sizeof(
			GrassConstantBuffer) %
		16 == 0);

	/**
	 * @brief GPU Skinning用ConstantBuffer
	 */
	struct BoneConstantBuffer
	{
		// Skinning用Bone Matrix
		Matrix4x4 BoneMatrices[MAX_BONES];

		// Skinningを行うか
		uint32_t HasSkinning = 0;

		// 16byte Alignment調整
		float Padding[3]{};
	};

	static_assert(
		sizeof(BoneConstantBuffer) % 16 == 0,
		"BoneConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief Outline描画用ConstantBuffer
	 */
	struct OutlineConstantBuffer
	{
		/**
		 * @brief World * View * Projection
		 */
		Matrix4x4 WorldViewProjection;

		/**
		 * @brief 法線変換用
		 */
		Matrix4x4 WorldInverseTranspose;

		/**
		 * @brief x : Outline Width
		 *        yzw : padding
		 */
		Vector4 OutlineParameters;

		/**
		 * @brief Outline Color
		 */
		Vector4 OutlineColor;
	};

	static_assert(
		sizeof(OutlineConstantBuffer) % 16 == 0,
		"OutlineConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief Shadow Map生成用ConstantBuffer
	 */
	struct ShadowCastConstantBuffer
	{
		Matrix4x4 LightWorldViewProjection;
	};

	static_assert(
		sizeof(ShadowCastConstantBuffer) % 16 == 0,
		"ShadowCastConstantBuffer must be 16-byte aligned.");


	/**
	 * @brief Shadowを受けるShader共通ConstantBuffer
	 */
	struct ShadowReceiveConstantBuffer
	{
		/**
		 * World Position
		 * → Light Clip Space
		 */
		Matrix4x4 LightViewProjection;

		/**
		 * x = Shadow Bias
		 * y = Shadow Strength
		 * z = Shadow Map Size
		 * w = unused
		 */
		Vector4 ShadowParameters;
	};

	static_assert(
		sizeof(ShadowReceiveConstantBuffer) % 16 == 0,
		"ShadowReceiveConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief 未探索領域Overlay用ConstantBuffer
	 */
	struct VisibilityOverlayConstantBuffer
	{
		/**
		 * @brief Local → World
		 */
		Matrix4x4 World;

		/**
		 * @brief World → View → Projection
		 */
		Matrix4x4 ViewProjection;

		/**
		 * x = CellSize
		 * y = ParchmentTiling
		 * z = HeightOffset
		 * w = unused
		 */
		Vector4 Parameters;
	};

	static_assert(
		sizeof(
			VisibilityOverlayConstantBuffer) %
		16 == 0,
		"VisibilityOverlayConstantBuffer must be 16-byte aligned.");

	/**
	 * @brief HLSLファイルをコンパイル
	 */
	bool CompileShader(
		const wchar_t* filePath,
		const char* entryPoint,
		const char* shaderModel,
		ComPtr<ID3DBlob>& shaderBlob)
	{
		UINT compileFlags = 0;

#ifdef _DEBUG

		// DebugBuildではShaderデバッグ情報を付ける
		compileFlags |=
			D3DCOMPILE_DEBUG |
			D3DCOMPILE_SKIP_OPTIMIZATION;

#endif

		ComPtr<ID3DBlob> errorBlob;

		const HRESULT hr =
			D3DCompileFromFile(
				filePath,
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				entryPoint,
				shaderModel,
				compileFlags,
				0,
				shaderBlob.GetAddressOf(),
				errorBlob.GetAddressOf());

		if (FAILED(hr))
		{
			// ShaderCompilerのエラーを
			// VisualStudio出力へ表示する
			if (errorBlob)
			{
				OutputDebugStringA(
					static_cast<const char*>(
						errorBlob->GetBufferPointer()));
			}

			return false;
		}

		return true;
	}

	/**
 * @brief Vector3を安全に正規化
 */
	Vector3 NormalizeDirection(
		const Vector3& value)
	{
		const float lengthSq =
			value.x * value.x +
			value.y * value.y +
			value.z * value.z;

		// ゼロベクトルを正規化しない
		if (lengthSq <= 0.000001f)
		{
			return Vector3(
				0.0f,
				-1.0f,
				0.0f);
		}

		const float inverseLength =
			1.0f /
			std::sqrt(lengthSq);

		return Vector3(
			value.x * inverseLength,
			value.y * inverseLength,
			value.z * inverseLength);
	}

	/**
	 * @brief 法線変換用の逆転置行列を生成
	 *
	 * @details
	 * World行列に非一様Scaleが含まれていても
	 * 法線方向が正しくなるようにする。
	 */
	Matrix4x4 CreateWorldInverseTranspose(
		const Matrix4x4& worldMatrix)
	{
		using namespace DirectX;

		const XMMATRIX world =
			XMLoadFloat4x4(
				&worldMatrix);

		const XMMATRIX inverse =
			XMMatrixInverse(
				nullptr,
				world);

		const XMMATRIX inverseTranspose =
			XMMatrixTranspose(
				inverse);

		Matrix4x4 result;

		XMStoreFloat4x4(
			&result,
			inverseTranspose);

		return result;
	}
}

//=====================================================
// staticメンバ
//=====================================================

D3D_FEATURE_LEVEL
Renderer::m_FeatureLevel =
D3D_FEATURE_LEVEL_11_0;

ComPtr<ID3D11Device>
Renderer::m_Device;

ComPtr<ID3D11DeviceContext>
Renderer::m_DeviceContext;

ComPtr<IDXGISwapChain>
Renderer::m_SwapChain;

ComPtr<ID3D11RenderTargetView>
Renderer::m_RenderTargetView;

ComPtr<ID3D11DepthStencilView>
Renderer::m_DepthStencilView;

ComPtr<ID3D11SamplerState>
Renderer::m_ModelSamplerState;


Matrix4x4 Renderer::m_ViewMatrix =
Matrix4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Matrix4x4 Renderer::m_ProjectionMatrix =
Matrix4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

ToonShading
Renderer::m_ToonShading{};

OutlineSetting
Renderer::m_OutlineSetting{};

ComPtr<ID3D11VertexShader>
Renderer::m_OutlineVertexShader;

ComPtr<ID3D11PixelShader>
Renderer::m_OutlinePixelShader;

ComPtr<ID3D11Buffer>
Renderer::m_OutlineConstantBuffer;

ComPtr<ID3D11RasterizerState>
Renderer::m_OutlineRasterizerState;

//=====================================================
// Lighting
//=====================================================

DirectionalLight
Renderer::m_DirectionalLight{};

Color Renderer::m_AmbientLight =
Color(
	0.5f,
	0.5f,
	0.5f,
	1.0f);

//====================
// Depth
//====================

ComPtr<ID3D11DepthStencilState>
Renderer::m_DepthStateEnable;

ComPtr<ID3D11DepthStencilState>
Renderer::m_DepthStateDisable;


//====================
// Blend
//====================

ComPtr<ID3D11BlendState>
Renderer::m_BlendStates[
	static_cast<size_t>(
		BlendState::Count)];


//====================
// Rasterizer
//====================

ComPtr<ID3D11RasterizerState>
Renderer::m_RasterizerSolidCull;

ComPtr<ID3D11RasterizerState>
Renderer::m_RasterizerSolidNoCull;

ComPtr<ID3D11RasterizerState>
Renderer::m_RasterizerWireframeCull;

ComPtr<ID3D11RasterizerState>
Renderer::m_RasterizerWireframeNoCull;

bool Renderer::m_CullingEnabled = true;

D3D11_FILL_MODE
Renderer::m_CurrentFillMode =
D3D11_FILL_SOLID;


//====================
// Model
//====================

ComPtr<ID3D11VertexShader>
Renderer::m_ModelVertexShader;

ComPtr<ID3D11PixelShader>
Renderer::m_ModelPixelShader;

ComPtr<ID3D11InputLayout>
Renderer::m_ModelInputLayout;

ComPtr<ID3D11Buffer>
Renderer::m_ModelConstantBuffer;

ComPtr<ID3D11Buffer>
Renderer::m_BoneConstantBuffer;

//====================
// Water
//====================

ComPtr<ID3D11VertexShader>
Renderer::m_WaterVertexShader;

ComPtr<ID3D11PixelShader>
Renderer::m_WaterPixelShader;

ComPtr<ID3D11InputLayout>
Renderer::m_WaterInputLayout;

ComPtr<ID3D11Buffer>
Renderer::m_WaterConstantBuffer;

//=====================================================
// Terrain Pipeline
//=====================================================

Microsoft::WRL::ComPtr<ID3D11VertexShader>
Renderer::m_TerrainVertexShader;

Microsoft::WRL::ComPtr<ID3D11PixelShader>
Renderer::m_TerrainPixelShader;

Microsoft::WRL::ComPtr<ID3D11InputLayout>
Renderer::m_TerrainInputLayout;

Microsoft::WRL::ComPtr<ID3D11Buffer>
Renderer::m_TerrainConstantBuffer;

//=====================================================
// Grass Pipeline
//=====================================================
Microsoft::WRL::ComPtr<ID3D11VertexShader>
Renderer::m_GrassVertexShader;

Microsoft::WRL::ComPtr<ID3D11PixelShader>
Renderer::m_GrassPixelShader;

Microsoft::WRL::ComPtr<ID3D11InputLayout>
Renderer::m_GrassInputLayout;

Microsoft::WRL::ComPtr<ID3D11Buffer>
Renderer::m_GrassConstantBuffer;

//=====================================================
// Shadow Map
//=====================================================

ComPtr<ID3D11Texture2D>
Renderer::m_ShadowMapTexture;

ComPtr<ID3D11DepthStencilView>
Renderer::m_ShadowMapDSV;

ComPtr<ID3D11ShaderResourceView>
Renderer::m_ShadowMapSRV;

ComPtr<ID3D11SamplerState>
Renderer::m_ShadowSampler;


//=====================================================
// Shadow Shader
//=====================================================

ComPtr<ID3D11VertexShader>
Renderer::m_ShadowStaticVertexShader;

ComPtr<ID3D11InputLayout>
Renderer::m_ShadowStaticInputLayout;

ComPtr<ID3D11VertexShader>
Renderer::m_ShadowSkinnedVertexShader;

ComPtr<ID3D11InputLayout>
Renderer::m_ShadowSkinnedInputLayout;

ComPtr<ID3D11VertexShader>
Renderer::m_ShadowGrassVertexShader;

ComPtr<ID3D11PixelShader>
Renderer::m_ShadowGrassPixelShader;

ComPtr<ID3D11InputLayout>
Renderer::m_ShadowGrassInputLayout;


//=====================================================
// Shadow ConstantBuffer
//=====================================================

ComPtr<ID3D11Buffer>
Renderer::m_ShadowCastConstantBuffer;

ComPtr<ID3D11Buffer>
Renderer::m_ShadowReceiveConstantBuffer;


//=====================================================
// Shadow Rasterizer
//=====================================================

ComPtr<ID3D11RasterizerState>
Renderer::m_ShadowRasterizerCull;

ComPtr<ID3D11RasterizerState>
Renderer::m_ShadowRasterizerNoCull;


//=====================================================
// Light Matrix
//=====================================================

Matrix4x4
Renderer::m_LightViewProjectionMatrix =
Matrix4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

//=====================================================
// Visibility Overlay
//=====================================================

ComPtr<ID3D11VertexShader>
Renderer::m_VisibilityOverlayVertexShader;

ComPtr<ID3D11PixelShader>
Renderer::m_VisibilityOverlayPixelShader;

ComPtr<ID3D11InputLayout>
Renderer::m_VisibilityOverlayInputLayout;

ComPtr<ID3D11Buffer>
Renderer::m_VisibilityOverlayConstantBuffer;

ComPtr<ID3D11SamplerState>
Renderer::m_VisibilitySampler;

//=====================================================
// Initialize
//=====================================================

bool Renderer::Init()
{
	OutputDebugStringA(
		"[Renderer::Init] START\n");
	//====================
	// Device
	//====================
	OutputDebugStringA(
		"[Renderer::Init] CreateDevice START\n");
	if (!CreateDevice())
	{
		return false;
	}
	OutputDebugStringA(
		"[Renderer::Init] CreateDevice OK\n");

	//====================
	// RenderTarget
	//====================
	OutputDebugStringA(
		"[Renderer::Init] RenderTarget START\n");
	if (!CreateRenderTarget())
	{
		return false;
	}
	OutputDebugStringA(
		"[Renderer::Init] RenderTarget OK\n");

	//====================
	// DepthBuffer
	//====================
	OutputDebugStringA(
		"[Renderer::Init] DepthStencil START\n");
	if (!CreateDepthStencil())
	{
		return false;
	}
	OutputDebugStringA(
		"[Renderer::Init] DepthStencil OK\n");

	// RenderTargetとDepthBufferを設定
	ID3D11RenderTargetView* renderTargets[] =
	{
		m_RenderTargetView.Get()
	};

	m_DeviceContext->OMSetRenderTargets(
		1,
		renderTargets,
		m_DepthStencilView.Get());

	//====================
	// Viewport
	//====================

	OutputDebugStringA(
		"[Renderer::Init] Viewport START\n");

	CreateViewport();

	OutputDebugStringA(
		"[Renderer::Init] Viewport OK\n");

	//====================
	// RenderState
	//====================

	OutputDebugStringA(
		"[Renderer::Init] RenderStates START\n");

	if (!CreateRenderStates())
	{
		OutputDebugStringA(
			"[Renderer::Init] RenderStates FAILED\n");

		return false;
	}

	OutputDebugStringA(
		"[Renderer::Init] RenderStates OK\n");

	//====================
	// Model描画Pipeline
	//====================
	OutputDebugStringA(
		"[Renderer::Init] ModelPipeline START\n");
	if (!CreateModelPipeline())
	{
		return false;
	}
	OutputDebugStringA(
		"[Renderer::Init] ModelPipeline OK\n");
	OutputDebugStringA(
		"[Renderer::Init] OutlinePipeline START\n");
	if (!CreateOutlinePipeline())
	{
		OutputDebugStringA(
			"[Renderer::Init] OutlinePipeline FAILED\n");
		return false;
	}
	OutputDebugStringA(
		"[Renderer::Init] OutlinePipeline OK\n");

	//====================
	// Water Pipeline
	//====================

	OutputDebugStringA(
		"[Renderer::Init] WaterPipeline START\n");

	if (!CreateWaterPipeline())
	{
		OutputDebugStringA(
			"[Renderer::Init] WaterPipeline FAILED\n");

		return false;
	}

	OutputDebugStringA(
		"[Renderer::Init] WaterPipeline OK\n");

	if (!CreateTerrainPipeline())
	{
		return false;
	}

	if (!CreateGrassPipeline())
	{
		return false;
	}

	//=====================================================
	// Shadow Pipeline
	//=====================================================

	if (!CreateShadowPipeline())
	{
		OutputDebugStringA(
			"[Renderer::Init] "
			"CreateShadowPipeline FAILED.\n");

		return false;
	}

	//=====================================================
	// Visibility Overlay Pipeline
	//=====================================================

	if (!CreateVisibilityOverlayPipeline())
	{
		OutputDebugStringA(
			"[Renderer::Init] "
			"CreateVisibilityOverlayPipeline FAILED.\n");

		return false;
	}

	//====================
	// 初期State
	//====================

	SetDepthEnable(true);

	SetBlendState(
		BlendState::None);

	SetCulling(true);

	SetFillMode(
		D3D11_FILL_SOLID);

	return true;
}


//=====================================================
// Dispose
//=====================================================

void Renderer::Dispose()
{
	//====================
	// Model
	//====================
	m_ModelSamplerState.Reset();

	m_BoneConstantBuffer.Reset();
	m_ModelConstantBuffer.Reset();
	m_ModelInputLayout.Reset();

	m_ModelPixelShader.Reset();
	m_ModelVertexShader.Reset();

	//====================
	// Water
	//====================

	m_WaterConstantBuffer.Reset();
	m_WaterInputLayout.Reset();

	m_WaterPixelShader.Reset();
	m_WaterVertexShader.Reset();

	m_TerrainConstantBuffer.Reset();
	m_TerrainInputLayout.Reset();
	m_TerrainPixelShader.Reset();
	m_TerrainVertexShader.Reset();

	m_GrassConstantBuffer.Reset();
	m_GrassInputLayout.Reset();
	m_GrassPixelShader.Reset();
	m_GrassVertexShader.Reset();

	//=====================================================
	// Shadow
	//=====================================================
	m_ShadowRasterizerNoCull.Reset();
	m_ShadowRasterizerCull.Reset();
	m_ShadowReceiveConstantBuffer.Reset();
	m_ShadowCastConstantBuffer.Reset();
	m_ShadowGrassInputLayout.Reset();
	m_ShadowGrassPixelShader.Reset();
	m_ShadowGrassVertexShader.Reset();
	m_ShadowSkinnedInputLayout.Reset();
	m_ShadowSkinnedVertexShader.Reset();
	m_ShadowStaticInputLayout.Reset();
	m_ShadowStaticVertexShader.Reset();
	m_ShadowSampler.Reset();
	m_ShadowMapSRV.Reset();
	m_ShadowMapDSV.Reset();
	m_ShadowMapTexture.Reset();

	//=====================================================
	// Visibility Overlay
	//=====================================================

	m_VisibilityOverlayConstantBuffer.Reset();

	m_VisibilityOverlayInputLayout.Reset();

	m_VisibilityOverlayPixelShader.Reset();

	m_VisibilityOverlayVertexShader.Reset();
	m_VisibilitySampler.Reset();

	//====================
	// Rasterizer
	//====================

	m_RasterizerWireframeNoCull.Reset();
	m_RasterizerWireframeCull.Reset();

	m_RasterizerSolidNoCull.Reset();
	m_RasterizerSolidCull.Reset();

	//====================
	// Blend
	//====================

	for (auto& blendState : m_BlendStates)
	{
		blendState.Reset();
	}

	//====================
	// Depth
	//====================

	m_DepthStateDisable.Reset();
	m_DepthStateEnable.Reset();

	//====================
	// RenderTarget
	//====================

	m_DepthStencilView.Reset();
	m_RenderTargetView.Reset();

	//====================
	// DXGI / D3D
	//====================

	m_SwapChain.Reset();

	m_DeviceContext.Reset();
	m_Device.Reset();
}


//=====================================================
// Frame
//=====================================================

void Renderer::Begin()
{
	if (!m_DeviceContext ||
		!m_RenderTargetView ||
		!m_DepthStencilView)
	{
		return;
	}

	// 背景色
	const float clearColor[4] =
	{
		0.00f,
		0.00f,
		0.00f,
		1.0f
	};

	// ColorBufferクリア
	m_DeviceContext->
		ClearRenderTargetView(
			m_RenderTargetView.Get(),
			clearColor);

	// DepthBufferクリア
	m_DeviceContext->
		ClearDepthStencilView(
			m_DepthStencilView.Get(),
			D3D11_CLEAR_DEPTH,
			1.0f,
			0);
}


void Renderer::End()
{
	if (!m_SwapChain)
	{
		return;
	}

	// VSync有効
	m_SwapChain->Present(
		1,
		0);
}


//=====================================================
// Device
//=====================================================

bool Renderer::CreateDevice()
{
	DXGI_SWAP_CHAIN_DESC desc{};

	desc.BufferCount = 1;

	desc.BufferDesc.Width =
		Window::GetWidth();

	desc.BufferDesc.Height =
		Window::GetHeight();

	// Direct2Dとの共有を行うため、
	// BGRA形式のBackBufferを使用する。
	desc.BufferDesc.Format =
		DXGI_FORMAT_B8G8R8A8_UNORM;

	desc.BufferDesc.RefreshRate.Numerator =
		60;

	desc.BufferDesc.RefreshRate.Denominator =
		1;

	desc.BufferUsage =
		DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.OutputWindow =
		Window::GetWindow();

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.Windowed = TRUE;

	UINT createFlags =
		D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG

	// DirectX DebugLayer
	// Windows側にGraphics Toolsが
	// インストールされている必要がある。
	createFlags |=
		D3D11_CREATE_DEVICE_DEBUG;

#endif

	const HRESULT hr =
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&desc,
			m_SwapChain.GetAddressOf(),
			m_Device.GetAddressOf(),
			&m_FeatureLevel,
			m_DeviceContext.GetAddressOf());

	if (FAILED(hr))
	{
#ifdef _DEBUG
		// DebugLayerが無い環境もあるため、
		// 必要ならReleaseBuildまたはFlags調整を行う。
#endif
		return false;
	}

	return true;
}


//=====================================================
// RenderTarget
//=====================================================

bool Renderer::CreateRenderTarget()
{
	if (!m_Device ||
		!m_SwapChain)
	{
		return false;
	}

	ComPtr<ID3D11Texture2D>
		backBuffer;

	const HRESULT getBufferResult =
		m_SwapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<void**>(
				backBuffer.GetAddressOf()));

	if (FAILED(getBufferResult))
	{
		return false;
	}

	const HRESULT createResult =
		m_Device->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			m_RenderTargetView.GetAddressOf());

	return SUCCEEDED(createResult);
}


//=====================================================
// DepthStencil
//=====================================================

bool Renderer::CreateDepthStencil()
{
	if (!m_Device)
	{
		return false;
	}

	//====================
	// DepthTexture
	//====================

	D3D11_TEXTURE2D_DESC textureDesc{};

	textureDesc.Width =
		Window::GetWidth();

	textureDesc.Height =
		Window::GetHeight();

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	textureDesc.Format =
		DXGI_FORMAT_D32_FLOAT;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage =
		D3D11_USAGE_DEFAULT;

	textureDesc.BindFlags =
		D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D>
		depthTexture;

	HRESULT hr =
		m_Device->CreateTexture2D(
			&textureDesc,
			nullptr,
			depthTexture.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//====================
	// DSV
	//====================

	D3D11_DEPTH_STENCIL_VIEW_DESC
		viewDesc{};

	viewDesc.Format =
		textureDesc.Format;

	viewDesc.ViewDimension =
		D3D11_DSV_DIMENSION_TEXTURE2D;

	viewDesc.Texture2D.MipSlice = 0;

	hr =
		m_Device->CreateDepthStencilView(
			depthTexture.Get(),
			&viewDesc,
			m_DepthStencilView.GetAddressOf());

	return SUCCEEDED(hr);
}


//=====================================================
// Viewport
//=====================================================

void Renderer::CreateViewport()
{
	D3D11_VIEWPORT viewport{};

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	viewport.Width =
		static_cast<float>(
			Window::GetWidth());

	viewport.Height =
		static_cast<float>(
			Window::GetHeight());

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_DeviceContext->RSSetViewports(
		1,
		&viewport);
}


//=====================================================
// RenderState
//=====================================================

bool Renderer::CreateRenderStates()
{
	//=================================================
	// DepthState
	//=================================================

	D3D11_DEPTH_STENCIL_DESC
		depthDesc{};

	depthDesc.DepthEnable = TRUE;

	depthDesc.DepthWriteMask =
		D3D11_DEPTH_WRITE_MASK_ALL;

	depthDesc.DepthFunc =
		D3D11_COMPARISON_LESS_EQUAL;

	depthDesc.StencilEnable =
		FALSE;

	HRESULT hr =
		m_Device->CreateDepthStencilState(
			&depthDesc,
			m_DepthStateEnable.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	depthDesc.DepthEnable = FALSE;

	depthDesc.DepthWriteMask =
		D3D11_DEPTH_WRITE_MASK_ZERO;

	hr =
		m_Device->CreateDepthStencilState(
			&depthDesc,
			m_DepthStateDisable.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// BlendState : None
	//=================================================

	D3D11_BLEND_DESC blendDesc{};

	blendDesc.RenderTarget[0].
		RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0].
		BlendEnable =
		FALSE;

	hr =
		m_Device->CreateBlendState(
			&blendDesc,
			m_BlendStates[
				static_cast<size_t>(
					BlendState::None)]
			.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// BlendState : Alpha
	//=================================================

	blendDesc.RenderTarget[0].
		BlendEnable =
		TRUE;

	blendDesc.RenderTarget[0].
		SrcBlend =
		D3D11_BLEND_SRC_ALPHA;

	blendDesc.RenderTarget[0].
		DestBlend =
		D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].
		BlendOp =
		D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].
		SrcBlendAlpha =
		D3D11_BLEND_ONE;

	blendDesc.RenderTarget[0].
		DestBlendAlpha =
		D3D11_BLEND_ZERO;

	blendDesc.RenderTarget[0].
		BlendOpAlpha =
		D3D11_BLEND_OP_ADD;

	hr =
		m_Device->CreateBlendState(
			&blendDesc,
			m_BlendStates[
				static_cast<size_t>(
					BlendState::Alpha)]
			.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// BlendState : Additive
	//=================================================

	blendDesc.RenderTarget[0].
		SrcBlend =
		D3D11_BLEND_SRC_ALPHA;

	blendDesc.RenderTarget[0].
		DestBlend =
		D3D11_BLEND_ONE;

	hr =
		m_Device->CreateBlendState(
			&blendDesc,
			m_BlendStates[
				static_cast<size_t>(
					BlendState::Additive)]
			.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// RasterizerState
	//=================================================

	auto createRasterizer =
		[](D3D11_FILL_MODE fillMode,
			D3D11_CULL_MODE cullMode,
			ComPtr<ID3D11RasterizerState>& result)
		-> bool
		{
			D3D11_RASTERIZER_DESC
				rasterizerDesc{};

			rasterizerDesc.FillMode =
				fillMode;

			rasterizerDesc.CullMode =
				cullMode;

			rasterizerDesc.FrontCounterClockwise =
				FALSE;

			rasterizerDesc.DepthClipEnable =
				TRUE;

			const HRESULT hr =
				m_Device->CreateRasterizerState(
					&rasterizerDesc,
					result.GetAddressOf());

			if (FAILED(hr))
			{
				OutputDebugStringA(
					"[Renderer] CreateRasterizerState failed.\n");

				return false;
			}

			return true;
		};

	//=================================================
	// 通常Rasterizer
	//=================================================

	if (!createRasterizer(
		D3D11_FILL_SOLID,
		D3D11_CULL_BACK,
		m_RasterizerSolidCull))
	{
		return false;
	}

	if (!createRasterizer(
		D3D11_FILL_SOLID,
		D3D11_CULL_NONE,
		m_RasterizerSolidNoCull))
	{
		return false;
	}

	if (!createRasterizer(
		D3D11_FILL_WIREFRAME,
		D3D11_CULL_BACK,
		m_RasterizerWireframeCull))
	{
		return false;
	}

	if (!createRasterizer(
		D3D11_FILL_WIREFRAME,
		D3D11_CULL_NONE,
		m_RasterizerWireframeNoCull))
	{
		return false;
	}

	//=================================================
	// Outline Rasterizer
	//=================================================
	//
	// Inverted Hull法では表面をカリングし、
	// 膨張させた裏面だけを描画する。
	//

	if (!createRasterizer(
		D3D11_FILL_SOLID,
		D3D11_CULL_FRONT,
		m_OutlineRasterizerState))
	{
		OutputDebugStringA(
			"[Renderer] Create OutlineRasterizerState failed.\n");

		return false;
	}

	return true;
}


//=====================================================
// Model Pipeline
//=====================================================

bool Renderer::CreateModelPipeline()
{
	//====================
	// ShaderCompile
	//====================

	ComPtr<ID3DBlob>
		vertexShaderBlob;

	ComPtr<ID3DBlob>
		pixelShaderBlob;

	if (!CompileShader(
		L"VS_Model.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	if (!CompileShader(
		L"PS_Model.hlsl",
		"main",
		"ps_5_0",
		pixelShaderBlob))
	{
		return false;
	}

	//====================
	// VertexShader
	//====================

	HRESULT hr =
		m_Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			nullptr,
			m_ModelVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//====================
	// PixelShader
	//====================

	hr =
		m_Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize(),
			nullptr,
			m_ModelPixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//====================
	// InputLayout
	//====================

	const D3D11_INPUT_ELEMENT_DESC
		inputElements[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					Position)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					Normal)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					TexCoord)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"BLENDINDICES",
			0,
			DXGI_FORMAT_R32G32B32A32_SINT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					BoneIndex)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"BLENDWEIGHT",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					BoneWeight)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		m_Device->CreateInputLayout(
			inputElements,
			static_cast<UINT>(
				std::size(
					inputElements)),
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			m_ModelInputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//====================
	// ConstantBuffer
	//====================

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(ModelConstantBuffer));

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_ModelConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] Create ModelConstantBuffer failed.\n");

		return false;
	}

	//=====================================================
	// Bone ConstantBuffer
	//=====================================================

	D3D11_BUFFER_DESC
		boneBufferDesc{};

	boneBufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(BoneConstantBuffer));

	boneBufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	boneBufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	boneBufferDesc.CPUAccessFlags = 0;

	hr =
		m_Device->CreateBuffer(
			&boneBufferDesc,
			nullptr,
			m_BoneConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] Create BoneConstantBuffer failed.\n");

		return false;
	}

	//====================
	// Texture Sampler
	//====================

	D3D11_SAMPLER_DESC samplerDesc{};

	samplerDesc.Filter =
		D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	samplerDesc.AddressU =
		D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.AddressV =
		D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.AddressW =
		D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD =
		D3D11_FLOAT32_MAX;

	hr =
		m_Device->CreateSamplerState(
			&samplerDesc,
			m_ModelSamplerState.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] Create ModelSamplerState failed.\n");

		return false;
	}

	return true;
}

bool Renderer::CreateOutlinePipeline()
{
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;

	//=================================================
	// Vertex Shader
	//=================================================

	if (!CompileShader(
		L"VS_Outline.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	HRESULT hr =
		m_Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			nullptr,
			m_OutlineVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Pixel Shader
	//=================================================

	if (!CompileShader(
		L"PS_Outline.hlsl",
		"main",
		"ps_5_0",
		pixelShaderBlob))
	{
		return false;
	}

	hr =
		m_Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize(),
			nullptr,
			m_OutlinePixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// ConstantBuffer
	//=================================================

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(OutlineConstantBuffer));

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_OutlineConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//=====================================================
// Water Pipeline
//=====================================================

bool Renderer::CreateWaterPipeline()
{
	if (!m_Device)
	{
		return false;
	}

	//=================================================
	// Shader Compile
	//=================================================

	ComPtr<ID3DBlob>
		vertexShaderBlob;

	ComPtr<ID3DBlob>
		pixelShaderBlob;

	if (!CompileShader(
		L"VS_Water.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Compile VS_Water failed.\n");

		return false;
	}

	if (!CompileShader(
		L"PS_Water.hlsl",
		"main",
		"ps_5_0",
		pixelShaderBlob))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Compile PS_Water failed.\n");

		return false;
	}

	//=================================================
	// Vertex Shader
	//=================================================

	HRESULT hr =
		m_Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			nullptr,
			m_WaterVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Create WaterVertexShader failed.\n");

		return false;
	}

	//=================================================
	// Pixel Shader
	//=================================================

	hr =
		m_Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize(),
			nullptr,
			m_WaterPixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Create WaterPixelShader failed.\n");

		return false;
	}

	//=================================================
	// Input Layout
	//=================================================
	//
	// Water Meshも現段階ではVertexDataを使用する。
	// Bone情報はWater Shaderでは使用しない。
	//=================================================

	const D3D11_INPUT_ELEMENT_DESC
		inputElements[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					Position)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					Normal)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					TexCoord)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"WATERDEPTH",
			0,
			DXGI_FORMAT_R32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					WaterVertex,
					WaterDepth)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		m_Device->CreateInputLayout(
			inputElements,
			static_cast<UINT>(
				std::size(
					inputElements)),
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			m_WaterInputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Create WaterInputLayout failed.\n");

		return false;
	}

	//=================================================
	// Constant Buffer
	//=================================================

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(
				WaterConstantBuffer));

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	bufferDesc.CPUAccessFlags =
		0;

	bufferDesc.MiscFlags =
		0;

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_WaterConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Create WaterConstantBuffer failed.\n");

		return false;
	}

	return true;
}

bool Renderer::CreateTerrainPipeline()
{
	//=================================================
	// Compile Shader
	//=================================================

	Microsoft::WRL::ComPtr<ID3DBlob>
		vertexShaderBlob;

	Microsoft::WRL::ComPtr<ID3DBlob>
		pixelShaderBlob;

	if (!CompileShader(
		L"VS_Terrain.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	if (!CompileShader(
		L"PS_Terrain.hlsl",
		"main",
		"ps_5_0",
		pixelShaderBlob))
	{
		return false;
	}

	//=================================================
	// Vertex Shader
	//=================================================

	HRESULT hr =
		m_Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			nullptr,
			m_TerrainVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Pixel Shader
	//=================================================

	hr =
		m_Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize(),
			nullptr,
			m_TerrainPixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Input Layout
	//=================================================
	const D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					TerrainVertex,
					Position)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					TerrainVertex,
					Normal)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					TerrainVertex,
					TexCoord)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"TERRAININDEX",
			0,

			// ps_4_0_level_9_3対応のためfloatとして渡す
			DXGI_FORMAT_R32_FLOAT,

			0,
			static_cast<UINT>(
				offsetof(
					TerrainVertex,
					TerrainIndex)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"NEIGHBORTERRAIN",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					TerrainVertex,
					NeighborTerrainIndices)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		m_Device->CreateInputLayout(
			inputLayout,
			static_cast<UINT>(
				std::size(
					inputLayout)),
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			m_TerrainInputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Constant Buffer
	//=================================================

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		sizeof(
			TerrainConstantBuffer);

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_TerrainConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Renderer::
CreateGrassPipeline()
{
	Microsoft::WRL::ComPtr<ID3DBlob>
		vertexShaderBlob;

	Microsoft::WRL::ComPtr<ID3DBlob>
		pixelShaderBlob;

	if (!CompileShader(
		L"VS_Grass.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	if (!CompileShader(
		L"PS_Grass.hlsl",
		"main",
		"ps_5_0",
		pixelShaderBlob))
	{
		return false;
	}

	HRESULT hr =
		m_Device->CreateVertexShader(
			vertexShaderBlob->
			GetBufferPointer(),
			vertexShaderBlob->
			GetBufferSize(),
			nullptr,
			m_GrassVertexShader.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	hr =
		m_Device->CreatePixelShader(
			pixelShaderBlob->
			GetBufferPointer(),
			pixelShaderBlob->
			GetBufferSize(),
			nullptr,
			m_GrassPixelShader.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	const D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		//=============================================
		// Slot 0 : GrassVertex
		//=============================================

		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					GrassVertex,
					Position)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					GrassVertex,
					TexCoord)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		//=============================================
		// Slot 1 : GrassInstanceData
		//=============================================

		{
			"INSTANCEPOSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Position)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},

		{
			"INSTANCEROTATION",
			0,
			DXGI_FORMAT_R32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Rotation)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},

		{
			"INSTANCESCALE",
			0,
			DXGI_FORMAT_R32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Scale)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},

		{
			"INSTANCERANDOM",
			0,
			DXGI_FORMAT_R32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Random)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		}
	};

	hr =
		m_Device->CreateInputLayout(
			inputLayout,
			static_cast<UINT>(
				std::size(
					inputLayout)),
			vertexShaderBlob->
			GetBufferPointer(),
			vertexShaderBlob->
			GetBufferSize(),
			m_GrassInputLayout.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		sizeof(
			GrassConstantBuffer);

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_GrassConstantBuffer.
			GetAddressOf());

	return
		SUCCEEDED(hr);
}

bool Renderer::CreateShadowPipeline()
{
	if (!m_Device)
	{
		return false;
	}

	//=================================================
	// Shadow Texture
	//=================================================

	D3D11_TEXTURE2D_DESC textureDesc{};

	textureDesc.Width =
		SHADOW_MAP_SIZE;

	textureDesc.Height =
		SHADOW_MAP_SIZE;

	textureDesc.MipLevels =
		1;

	textureDesc.ArraySize =
		1;

	// DSV / SRVで別Formatとして扱う
	textureDesc.Format =
		DXGI_FORMAT_R32_TYPELESS;

	textureDesc.SampleDesc.Count =
		1;

	textureDesc.SampleDesc.Quality =
		0;

	textureDesc.Usage =
		D3D11_USAGE_DEFAULT;

	textureDesc.BindFlags =
		D3D11_BIND_DEPTH_STENCIL |
		D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr =
		m_Device->CreateTexture2D(
			&textureDesc,
			nullptr,
			m_ShadowMapTexture.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// DSV
	//=================================================

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Format =
		DXGI_FORMAT_D32_FLOAT;

	dsvDesc.ViewDimension =
		D3D11_DSV_DIMENSION_TEXTURE2D;

	dsvDesc.Texture2D.MipSlice =
		0;

	hr =
		m_Device->CreateDepthStencilView(
			m_ShadowMapTexture.Get(),
			&dsvDesc,
			m_ShadowMapDSV.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// SRV
	//=================================================

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format =
		DXGI_FORMAT_R32_FLOAT;

	srvDesc.ViewDimension =
		D3D11_SRV_DIMENSION_TEXTURE2D;

	srvDesc.Texture2D.MostDetailedMip =
		0;

	srvDesc.Texture2D.MipLevels =
		1;

	hr =
		m_Device->CreateShaderResourceView(
			m_ShadowMapTexture.Get(),
			&srvDesc,
			m_ShadowMapSRV.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Comparison Sampler
	//=================================================
	D3D11_SAMPLER_DESC samplerDesc{};

	samplerDesc.Filter =
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	samplerDesc.AddressU =
		D3D11_TEXTURE_ADDRESS_BORDER;

	samplerDesc.AddressV =
		D3D11_TEXTURE_ADDRESS_BORDER;

	samplerDesc.AddressW =
		D3D11_TEXTURE_ADDRESS_BORDER;

	// SamplerComparisonState用。
	// Shadow Map上のDepthと比較する。
	samplerDesc.ComparisonFunc =
		D3D11_COMPARISON_LESS_EQUAL;

	// Shadow Map外は「光が当たっている」として扱う
	samplerDesc.BorderColor[0] =
		1.0f;

	samplerDesc.BorderColor[1] =
		1.0f;

	samplerDesc.BorderColor[2] =
		1.0f;

	samplerDesc.BorderColor[3] =
		1.0f;

	samplerDesc.MinLOD =
		0.0f;

	samplerDesc.MaxLOD =
		D3D11_FLOAT32_MAX;

	hr =
		m_Device->CreateSamplerState(
			&samplerDesc,
			m_ShadowSampler.
			ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugStringA(
			"[Renderer] "
			"Create Shadow Comparison Sampler failed.\n");

		return false;
	}

	//=================================================
	// Rasterizer
	//=================================================

	auto createShadowRasterizer =
		[](D3D11_CULL_MODE cullMode,
			ComPtr<ID3D11RasterizerState>& output)
		-> bool
		{
			D3D11_RASTERIZER_DESC desc{};

			desc.FillMode =
				D3D11_FILL_SOLID;

			desc.CullMode =
				cullMode;

			desc.FrontCounterClockwise =
				FALSE;

			desc.DepthClipEnable =
				TRUE;

			// Shadow Acne軽減
			desc.DepthBias =
				1000;

			desc.SlopeScaledDepthBias =
				1.5f;

			desc.DepthBiasClamp =
				0.0f;

			return SUCCEEDED(
				m_Device->CreateRasterizerState(
					&desc,
					output.GetAddressOf()));
		};

	if (!createShadowRasterizer(
		D3D11_CULL_BACK,
		m_ShadowRasterizerCull))
	{
		return false;
	}

	// Grassは両面
	if (!createShadowRasterizer(
		D3D11_CULL_NONE,
		m_ShadowRasterizerNoCull))
	{
		return false;
	}

	//=================================================
	// Static Shadow Shader
	//=================================================

	ComPtr<ID3DBlob>
		staticShaderBlob;

	if (!CompileShader(
		L"VS_ShadowStatic.hlsl",
		"main",
		"vs_5_0",
		staticShaderBlob))
	{
		return false;
	}

	hr =
		m_Device->CreateVertexShader(
			staticShaderBlob->GetBufferPointer(),
			staticShaderBlob->GetBufferSize(),
			nullptr,
			m_ShadowStaticVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	// POSITIONは全対象Vertexの先頭に存在する
	const D3D11_INPUT_ELEMENT_DESC
		staticLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		m_Device->CreateInputLayout(
			staticLayout,
			static_cast<UINT>(
				std::size(staticLayout)),
			staticShaderBlob->GetBufferPointer(),
			staticShaderBlob->GetBufferSize(),
			m_ShadowStaticInputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Skinned Shadow Shader
	//=================================================

	ComPtr<ID3DBlob>
		skinnedShaderBlob;

	if (!CompileShader(
		L"VS_ShadowSkinned.hlsl",
		"main",
		"vs_5_0",
		skinnedShaderBlob))
	{
		return false;
	}

	hr =
		m_Device->CreateVertexShader(
			skinnedShaderBlob->GetBufferPointer(),
			skinnedShaderBlob->GetBufferSize(),
			nullptr,
			m_ShadowSkinnedVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	const D3D11_INPUT_ELEMENT_DESC
		skinnedLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					Position)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"BLENDINDICES",
			0,
			DXGI_FORMAT_R32G32B32A32_SINT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					BoneIndex)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"BLENDWEIGHT",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					VertexData,
					BoneWeight)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		m_Device->CreateInputLayout(
			skinnedLayout,
			static_cast<UINT>(
				std::size(
					skinnedLayout)),
			skinnedShaderBlob->GetBufferPointer(),
			skinnedShaderBlob->GetBufferSize(),
			m_ShadowSkinnedInputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Grass Shadow Shader
	//=================================================

	ComPtr<ID3DBlob>
		grassVSBlob;

	ComPtr<ID3DBlob>
		grassPSBlob;

	if (!CompileShader(
		L"VS_ShadowGrass.hlsl",
		"main",
		"vs_5_0",
		grassVSBlob))
	{
		return false;
	}

	if (!CompileShader(
		L"PS_ShadowGrass.hlsl",
		"main",
		"ps_5_0",
		grassPSBlob))
	{
		return false;
	}

	hr =
		m_Device->CreateVertexShader(
			grassVSBlob->GetBufferPointer(),
			grassVSBlob->GetBufferSize(),
			nullptr,
			m_ShadowGrassVertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	hr =
		m_Device->CreatePixelShader(
			grassPSBlob->GetBufferPointer(),
			grassPSBlob->GetBufferSize(),
			nullptr,
			m_ShadowGrassPixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	// Grass通常描画と同じVertex構造
	const D3D11_INPUT_ELEMENT_DESC
		grassLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					GrassVertex,
					Position)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					GrassVertex,
					TexCoord)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"INSTANCEPOSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Position)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},

		{
			"INSTANCEROTATION",
			0,
			DXGI_FORMAT_R32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Rotation)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},

		{
			"INSTANCESCALE",
			0,
			DXGI_FORMAT_R32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Scale)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		},

		{
			"INSTANCERANDOM",
			0,
			DXGI_FORMAT_R32_FLOAT,
			1,
			static_cast<UINT>(
				offsetof(
					GrassInstanceData,
					Random)),
			D3D11_INPUT_PER_INSTANCE_DATA,
			1
		}
	};

	hr =
		m_Device->CreateInputLayout(
			grassLayout,
			static_cast<UINT>(
				std::size(
					grassLayout)),
			grassVSBlob->GetBufferPointer(),
			grassVSBlob->GetBufferSize(),
			m_ShadowGrassInputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Shadow Cast ConstantBuffer
	//=================================================

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(
				ShadowCastConstantBuffer));

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_ShadowCastConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Shadow Receive ConstantBuffer
	//=================================================

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(
				ShadowReceiveConstantBuffer));

	hr =
		m_Device->CreateBuffer(
			&bufferDesc,
			nullptr,
			m_ShadowReceiveConstantBuffer.GetAddressOf());

	return SUCCEEDED(hr);
}

bool Renderer::
CreateVisibilityOverlayPipeline()
{
	if (!m_Device)
	{
		return false;
	}

	//=================================================
	// Shader Compile
	//=================================================

	ComPtr<ID3DBlob>
		vertexShaderBlob;

	ComPtr<ID3DBlob>
		pixelShaderBlob;

	if (!CompileShader(
		L"VS_VisibilityOverlay.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	if (!CompileShader(
		L"PS_VisibilityOverlay.hlsl",
		"main",
		"ps_5_0",
		pixelShaderBlob))
	{
		return false;
	}

	//=================================================
	// Vertex Shader
	//=================================================

	HRESULT hr =
		m_Device->
		CreateVertexShader(
			vertexShaderBlob->
			GetBufferPointer(),

			vertexShaderBlob->
			GetBufferSize(),

			nullptr,

			m_VisibilityOverlayVertexShader.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Pixel Shader
	//=================================================

	hr =
		m_Device->
		CreatePixelShader(
			pixelShaderBlob->
			GetBufferPointer(),

			pixelShaderBlob->
			GetBufferSize(),

			nullptr,

			m_VisibilityOverlayPixelShader.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Input Layout
	//=================================================
	//
	// Ground:
	// TerrainVertexの先頭 = Position
	//
	// Water:
	// WaterVertexの先頭 = Position
	//
	// そのためPOSITIONだけ読めば
	// 同一Pipelineで両方描画できる。
	//=================================================

	const D3D11_INPUT_ELEMENT_DESC
		inputLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		m_Device->
		CreateInputLayout(
			inputLayout,

			static_cast<UINT>(
				std::size(
					inputLayout)),

			vertexShaderBlob->
			GetBufferPointer(),

			vertexShaderBlob->
			GetBufferSize(),

			m_VisibilityOverlayInputLayout.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Constant Buffer
	//=================================================

	D3D11_BUFFER_DESC
		bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(
				VisibilityOverlayConstantBuffer));

	bufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	bufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	bufferDesc.CPUAccessFlags =
		0;

	hr =
		m_Device->
		CreateBuffer(
			&bufferDesc,
			nullptr,
			m_VisibilityOverlayConstantBuffer.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Visibility Linear Sampler
	//=================================================
	//
	// Gameplay上のVisibilityはGrid単位だが、
	// 描画上は境界を滑らかに補間する。
	//
	// CLAMPにすることでMap端で反対側へ
	// Visibilityが回り込まないようにする。
	//=================================================

	D3D11_SAMPLER_DESC
		visibilitySamplerDesc{};

	visibilitySamplerDesc.Filter =
		D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	visibilitySamplerDesc.AddressU =
		D3D11_TEXTURE_ADDRESS_CLAMP;

	visibilitySamplerDesc.AddressV =
		D3D11_TEXTURE_ADDRESS_CLAMP;

	visibilitySamplerDesc.AddressW =
		D3D11_TEXTURE_ADDRESS_CLAMP;

	visibilitySamplerDesc.MinLOD =
		0.0f;

	visibilitySamplerDesc.MaxLOD =
		D3D11_FLOAT32_MAX;

	hr =
		m_Device->
		CreateSamplerState(
			&visibilitySamplerDesc,
			m_VisibilitySampler.
			GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//=====================================================
// Model描画
//=====================================================

void Renderer::DrawMesh(
	const MeshBuffer& mesh,
	const MaterialData& material,
	const Matrix4x4& worldMatrix,
	std::span<const Matrix4x4> boneMatrices)
{
	if (!m_DeviceContext ||
		!mesh.IsValid())
	{
		return;
	}

	m_DeviceContext->
		RSSetState(
			m_RasterizerSolidCull.Get());

	//=================================================
	// Camera
	//=================================================
	

	//=================================================
	// ConstantBuffer
	//=================================================

	ModelConstantBuffer
		constantBuffer{};

	constantBuffer.World =
		worldMatrix.Transpose();
	//=================================================
	// World View Projection
	//=================================================

	constantBuffer.WorldViewProjection =
		(
			worldMatrix *
			m_ViewMatrix *
			m_ProjectionMatrix
			).Transpose();

	//=================================================
	// Normal Matrix
	//=================================================
	//
	// PositionとNormalでは変換方法が異なる。
	// NormalにはWorld行列の逆転置行列を使用する。
	//

	const Matrix4x4
		worldInverseTranspose =
		CreateWorldInverseTranspose(
			worldMatrix);

	constantBuffer.WorldInverseTranspose =
		worldInverseTranspose.Transpose();

	//=================================================
	// Material
	//=================================================

	constantBuffer.DiffuseColor =
		material.Diffuse;

	//=================================================
	// Directional Light
	//=================================================

	constantBuffer.LightDirectionIntensity =
		Vector4(
			m_DirectionalLight.Direction.x,
			m_DirectionalLight.Direction.y,
			m_DirectionalLight.Direction.z,
			m_DirectionalLight.Intensity);

	constantBuffer.LightColor =
		Vector4(
			m_DirectionalLight.LightColor.x,
			m_DirectionalLight.LightColor.y,
			m_DirectionalLight.LightColor.z,
			m_DirectionalLight.LightColor.w);

	//=================================================
	// Ambient Light
	//=================================================

	constantBuffer.AmbientLightColor =
		Vector4(
			m_AmbientLight.x,
			m_AmbientLight.y,
			m_AmbientLight.z,
			m_AmbientLight.w);

	//=================================================
	// Toon Shading
	//=================================================

	constantBuffer.ToonParameters =
		Vector4(
			m_ToonShading.HighlightThreshold,
			m_ToonShading.ShadowThreshold,
			m_ToonShading.MidToneIntensity,
			m_ToonShading.ShadowIntensity);

	//=================================================
	// Texture
	//=================================================

	constantBuffer.HasTexture =
		(material.Texture &&
			material.Texture->IsValid())
		? 1u
		: 0u;

	m_DeviceContext->
		UpdateSubresource(
			m_ModelConstantBuffer.Get(),
			0,
			nullptr,
			&constantBuffer,
			0,
			0);

	//=====================================================
	// Bone ConstantBuffer
	//=====================================================

	BoneConstantBuffer
		boneConstantBuffer{};

	// Bone行列が存在する場合のみSkinning
	if (!boneMatrices.empty())
	{
		if (boneMatrices.size() >
			MAX_BONES)
		{
			OutputDebugStringA(
				"[Renderer::DrawMesh] Bone count exceeds MAX_BONES.\n");

			return;
		}

		boneConstantBuffer.HasSkinning = 1;

		for (size_t i = 0;
			i < boneMatrices.size();
			++i)
		{
			// HLSLへ渡すためTranspose
			boneConstantBuffer.
				BoneMatrices[i] =
				boneMatrices[i].
				Transpose();
		}
	}
	else
	{
		// Static Modelの場合
		boneConstantBuffer.HasSkinning = 0;
	}

	m_DeviceContext->
		UpdateSubresource(
			m_BoneConstantBuffer.Get(),
			0,
			nullptr,
			&boneConstantBuffer,
			0,
			0);

	//=================================================
	// InputAssembler
	//=================================================

	m_DeviceContext->
		IASetInputLayout(
			m_ModelInputLayout.Get());

	//=================================================
	// Shader
	//=================================================

	m_DeviceContext->VSSetShader(
		m_ModelVertexShader.Get(),
		nullptr,
		0);

	m_DeviceContext->PSSetShader(
		m_ModelPixelShader.Get(),
		nullptr,
		0);

	ID3D11Buffer* constantBuffers[] =
	{
		m_ModelConstantBuffer.Get()
	};

	// VertexShader b0
	m_DeviceContext->
		VSSetConstantBuffers(
			0,
			1,
			constantBuffers);

	// PixelShader b0
	m_DeviceContext->
		PSSetConstantBuffers(
			0,
			1,
			constantBuffers);

	//=====================================================
	// Bone ConstantBuffer
	//=====================================================

	ID3D11Buffer* boneBuffers[] =
	{
		m_BoneConstantBuffer.Get()
	};

	// VertexShader b1
	m_DeviceContext->
		VSSetConstantBuffers(
			1,
			1,
			boneBuffers);

	//=================================================
	// Texture
	//=================================================

	ID3D11ShaderResourceView*
		textureView = nullptr;

	if (material.Texture &&
		material.Texture->IsValid())
	{
		textureView =
			material.Texture->
			GetShaderResourceView();
	}

	// PixelShader t0
	m_DeviceContext->
		PSSetShaderResources(
			0,
			1,
			&textureView);

	// PixelShader s0
	ID3D11SamplerState*
		sampler =
		m_ModelSamplerState.Get();

	m_DeviceContext->
		PSSetSamplers(
			0,
			1,
			&sampler);

	//=================================================
	// Mesh
	//=================================================

	mesh.Bind(
		m_DeviceContext.Get());

	mesh.Draw(
		m_DeviceContext.Get());
}

//=====================================================
// Water Draw
//=====================================================
void Renderer::DrawWater(
	const MeshBuffer& mesh,
	const Matrix4x4& worldMatrix,
	const WaterRenderSettings& settings,
	const Texture* normalMapA,
	const Texture* normalMapB,
	float time,
	const Vector3& cameraPosition)
{
	//=================================================
	// Validation
	//=================================================

	if (!mesh.IsValid())
	{
		return;
	}

	if (!m_WaterVertexShader ||
		!m_WaterPixelShader ||
		!m_WaterInputLayout ||
		!m_WaterConstantBuffer)
	{
		return;
	}

	ID3D11DeviceContext* context =
		GetDeviceContext();

	if (!context)
	{
		return;
	}

	//=================================================
	// Normal Map Validation
	//=================================================

	const bool hasNormalMapA =
		normalMapA &&
		normalMapA->IsValid();

	const bool hasNormalMapB =
		normalMapB &&
		normalMapB->IsValid();

	//=================================================
	// Flow Direction
	//=================================================

	Vector2 flowDirectionA =
		settings.FlowDirectionA;

	Vector2 flowDirectionB =
		settings.FlowDirectionB;

	// 方向ベクトルが0でない場合だけ正規化
	if (flowDirectionA.LengthSquared() >
		0.000001f)
	{
		flowDirectionA.Normalize();
	}

	if (flowDirectionB.LengthSquared() >
		0.000001f)
	{
		flowDirectionB.Normalize();
	}

	//=================================================
	// Constant Buffer
	//=================================================

	WaterConstantBuffer constantBuffer{};

	const Matrix4x4 wvp =
		worldMatrix *
		m_ViewMatrix *
		m_ProjectionMatrix;

	constantBuffer.World =
		worldMatrix.Transpose();

	constantBuffer.WorldViewProjection =
		wvp.Transpose();

	constantBuffer.WaterColor =
		settings.Color;

	constantBuffer.FlowA =
		Vector4(
			flowDirectionA.x,
			flowDirectionA.y,
			settings.FlowSpeedA,
			settings.TilingA);

	constantBuffer.FlowB =
		Vector4(
			flowDirectionB.x,
			flowDirectionB.y,
			settings.FlowSpeedB,
			settings.TilingB);

	constantBuffer.NormalParameters =
		Vector4(
			settings.NormalStrength,
			time,
			hasNormalMapA ? 1.0f : 0.0f,
			hasNormalMapB ? 1.0f : 0.0f);

	//=================================================
	// Directional Light
	//=================================================

	constantBuffer.LightDirectionIntensity =
		Vector4(
			m_DirectionalLight.Direction.x,
			m_DirectionalLight.Direction.y,
			m_DirectionalLight.Direction.z,
			m_DirectionalLight.Intensity);

	constantBuffer.LightColor =
		m_DirectionalLight.LightColor;

	constantBuffer.AmbientLightColor =
		m_AmbientLight;

	//=================================================
	// Camera
	//=================================================

	constantBuffer.CameraPosition =
		Vector4(
			cameraPosition.x,
			cameraPosition.y,
			cameraPosition.z,
			1.0f);

	//=====================================================
	// Water Depth
	//=====================================================

	constantBuffer.ShallowWaterColor =
		settings.ShallowColor;

	constantBuffer.DeepWaterColor =
		settings.DeepColor;

	constantBuffer.DepthParameters =
		Vector4(
			settings.ShallowDepth,
			settings.DeepDepth,
			settings.ShallowAlpha,
			settings.DeepAlpha);

	//=================================================
	// Reflection
	//=================================================

	constantBuffer.ReflectionParameters =
		Vector4(
			settings.SpecularStrength,
			settings.SpecularPower,
			settings.FresnelStrength,
			settings.FresnelPower);

	constantBuffer.FresnelColor =
		settings.FresnelColor;

	//=================================================
	// Upload
	//=================================================

	context->UpdateSubresource(
		m_WaterConstantBuffer.Get(),
		0,
		nullptr,
		&constantBuffer,
		0,
		0);

	//=================================================
	// Pipeline
	//=================================================

	context->IASetInputLayout(
		m_WaterInputLayout.Get());

	context->VSSetShader(
		m_WaterVertexShader.Get(),
		nullptr,
		0);

	context->PSSetShader(
		m_WaterPixelShader.Get(),
		nullptr,
		0);

	ID3D11Buffer* constantBuffers[] =
	{
		m_WaterConstantBuffer.Get()
	};

	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	context->PSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	//=================================================
	// Normal Maps
	//=================================================

	ID3D11ShaderResourceView* normalSRVs[2] =
	{
		hasNormalMapA
			? normalMapA->GetShaderResourceView()
			: nullptr,

		hasNormalMapB
			? normalMapB->GetShaderResourceView()
			: nullptr
	};

	context->PSSetShaderResources(
		0,
		2,
		normalSRVs);

	// 既存Model用Samplerを使用
	ID3D11SamplerState* samplers[] =
	{
		m_ModelSamplerState.Get()
	};

	context->PSSetSamplers(
		0,
		1,
		samplers);

	//=================================================
	// Draw
	//=================================================

	mesh.Bind(
		context);

	mesh.Draw(
		context);

	//=================================================
	// Unbind SRV
	//=================================================
	//
	// 後でRenderTarget等として使用したときの
	// D3D11 Resource競合を避ける。
	//=================================================

	ID3D11ShaderResourceView* nullSRVs[2] =
	{
		nullptr,
		nullptr
	};

	context->PSSetShaderResources(
		0,
		2,
		nullSRVs);
}

void Renderer::DrawTerrain(
	const MeshBuffer& mesh,
	const Matrix4x4& worldMatrix,
	const TerrainRenderSettings& settings,
	const Texture* grassTexture,
	const Texture* dirtTexture,
	const Texture* rockTexture,
	const Texture* sandTexture,
	ID3D11ShaderResourceView* visibilitySRV)
{
	if (!mesh.IsValid())
	{
		return;
	}

	if (!grassTexture ||
		!dirtTexture ||
		!rockTexture ||
		!sandTexture)
	{
		return;
	}

	if (!grassTexture->IsValid() ||
		!dirtTexture->IsValid() ||
		!rockTexture->IsValid() ||
		!sandTexture->IsValid())
	{
		return;
	}

	if (!m_TerrainVertexShader ||
		!m_TerrainPixelShader ||
		!m_TerrainInputLayout ||
		!m_TerrainConstantBuffer)
	{
		return;
	}

	ID3D11DeviceContext* context =
		GetDeviceContext();

	if (!context)
	{
		return;
	}

	//=================================================
	// Constant Buffer
	//=================================================

	TerrainConstantBuffer constantBuffer{};

	const Matrix4x4 wvp =
		worldMatrix *
		m_ViewMatrix *
		m_ProjectionMatrix;

	constantBuffer.World =
		worldMatrix.Transpose();

	constantBuffer.WorldViewProjection =
		wvp.Transpose();

	constantBuffer.TerrainParameters =
		Vector4(
			settings.Tiling,
			settings.CellSize,
			settings.ExploredBrightness,
			settings.ParchmentTiling);

	constantBuffer.TerrainColor =
		settings.Color;

	//=================================================
	// Lighting
	//=================================================
	constantBuffer.LightDirectionIntensity =
		Vector4(
			m_DirectionalLight.Direction.x,
			m_DirectionalLight.Direction.y,
			m_DirectionalLight.Direction.z,
			m_DirectionalLight.Intensity);

	constantBuffer.LightColor =
		m_DirectionalLight.LightColor;

	constantBuffer.AmbientLightColor =
		m_AmbientLight;

	constantBuffer.ToonParameters =
		Vector4(
			m_ToonShading.HighlightThreshold,
			m_ToonShading.ShadowThreshold,
			m_ToonShading.MidToneIntensity,
			m_ToonShading.ShadowIntensity);

	context->UpdateSubresource(
		m_TerrainConstantBuffer.Get(),
		0,
		nullptr,
		&constantBuffer,
		0,
		0);

	//=================================================
	// Pipeline
	//=================================================

	context->IASetInputLayout(
		m_TerrainInputLayout.Get());

	context->VSSetShader(
		m_TerrainVertexShader.Get(),
		nullptr,
		0);

	context->PSSetShader(
		m_TerrainPixelShader.Get(),
		nullptr,
		0);

	ID3D11Buffer* constantBuffers[] =
	{
		m_TerrainConstantBuffer.Get()
	};

	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	context->PSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	//=================================================
	// Terrain Textures
	//=================================================

	ID3D11ShaderResourceView* terrainSRVs[5] =
	{
		// t0
		grassTexture->
			GetShaderResourceView(),

		// t1
		dirtTexture->
			GetShaderResourceView(),

		// t2
		rockTexture->
			GetShaderResourceView(),

		// t3
		sandTexture->
			GetShaderResourceView(),

		// t4
		// VisibilityMapをGPUへ渡したTexture
		visibilitySRV
	};

	context->PSSetShaderResources(
		0,
		5,
		terrainSRVs);

	ID3D11SamplerState* sampler =
		m_ModelSamplerState.Get();

	context->PSSetSamplers(
		0,
		1,
		&sampler);

	//=================================================
	// Draw
	//=================================================

	mesh.Bind(
		context);

	mesh.Draw(
		context);

	//=================================================
	// Unbind
	//=================================================

	ID3D11ShaderResourceView* nullSRVs[5] =
	{
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};

	context->PSSetShaderResources(
		0,
		5,
		nullSRVs);
}

void Renderer::
DrawGrassInstanced(
	const MeshBuffer& mesh,
	ID3D11Buffer* instanceBuffer,
	uint32_t instanceCount,
	const Texture* grassTexture,
	ID3D11ShaderResourceView* visibilitySRV,
	float cellSize,
	float time,
	const Vector2& windDirection,
	float windStrength,
	float windSpeed)
{
	if (!mesh.IsValid() ||
		!instanceBuffer ||
		instanceCount == 0 ||
		!grassTexture ||
		!grassTexture->IsValid() ||
		!visibilitySRV)
	{
		return;
	}

	ID3D11DeviceContext* context =
		GetDeviceContext();

	if (!context)
	{
		return;
	}

	//=================================================
	// Constant Buffer
	//=================================================

	GrassConstantBuffer constantBuffer{};

	const Matrix4x4 viewProjection =
		m_ViewMatrix *
		m_ProjectionMatrix;

	constantBuffer.ViewProjection =
		viewProjection.Transpose();

	constantBuffer.GrassParameters =
		Vector4(
			time,
			windStrength,
			windSpeed,
			0.40f);	// AlphaClip threshold

	constantBuffer.WindParameters =
		Vector4(
			windDirection.x,
			windDirection.y,
			0.0f,
			0.0f);

	// Visibility判定で
	// World座標 → Grid座標へ変換するために使用
	constantBuffer.VisibilityParameters =
		Vector4(
			cellSize,
			0.0f,
			0.0f,
			0.0f);

	context->UpdateSubresource(
		m_GrassConstantBuffer.Get(),
		0,
		nullptr,
		&constantBuffer,
		0,
		0);

	//=================================================
	// Pipeline
	//=================================================

	context->IASetInputLayout(
		m_GrassInputLayout.Get());

	context->VSSetShader(
		m_GrassVertexShader.Get(),
		nullptr,
		0);

	context->PSSetShader(
		m_GrassPixelShader.Get(),
		nullptr,
		0);

	ID3D11Buffer* constantBuffers[] =
	{
		m_GrassConstantBuffer.Get()
	};

	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	context->PSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	//=================================================
	// Grass Texture : t0
	//=================================================

	ID3D11ShaderResourceView* grassSRV =
		grassTexture->
		GetShaderResourceView();

	context->PSSetShaderResources(
		0,
		1,
		&grassSRV);

	//=================================================
	// Visibility Texture : t4
	//=================================================
	//
	// PS_Grass.hlsl:
	//
	// Texture2D VisibilityTexture :
	//     register(t4);
	//
	// と対応する。
	//=================================================

	context->PSSetShaderResources(
		4,
		1,
		&visibilitySRV);

	//=================================================
	// Sampler
	//=================================================

	ID3D11SamplerState* sampler =
		m_ModelSamplerState.Get();

	context->PSSetSamplers(
		0,
		1,
		&sampler);

	//=================================================
	// Base Mesh
	//=================================================

	mesh.Bind(
		context);

	//=================================================
	// Instance Buffer
	//=================================================

	ID3D11Buffer* instanceBuffers[] =
	{
		instanceBuffer
	};

	const UINT instanceStrides[] =
	{
		sizeof(
			GrassInstanceData)
	};

	const UINT instanceOffsets[] =
	{
		0
	};

	context->IASetVertexBuffers(
		1,
		1,
		instanceBuffers,
		instanceStrides,
		instanceOffsets);

	//=================================================
	// Render State
	//=================================================

	// 草は両面描画
	SetCulling(
		false);

	// Alpha Clipなので
	// Alpha Blendは使用しない
	SetBlendState(
		BlendState::None);

	//=================================================
	// Draw
	//=================================================

	context->DrawIndexedInstanced(
		mesh.GetIndexCount(),
		instanceCount,
		0,
		0,
		0);

	//=================================================
	// State Reset
	//=================================================

	SetCulling(
		true);

	//=================================================
	// SRV Unbind
	//=================================================

	ID3D11ShaderResourceView* nullSRV =
		nullptr;

	// t0 : Grass Texture
	context->PSSetShaderResources(
		0,
		1,
		&nullSRV);

	// t4 : Visibility Texture
	context->PSSetShaderResources(
		4,
		1,
		&nullSRV);
}


void Renderer::
DrawVisibilityOverlay(
	const MeshBuffer& mesh,
	const Matrix4x4& worldMatrix,
	const Texture* parchmentTexture,
	ID3D11ShaderResourceView* visibilitySRV,
	float cellSize,
	float time,
	float parchmentTiling,
	float heightOffset)
{
	//=================================================
	// Validation
	//=================================================

	if (!mesh.IsValid() ||
		!parchmentTexture ||
		!parchmentTexture->IsValid() ||
		!visibilitySRV)
	{
		return;
	}

	if (!m_VisibilityOverlayVertexShader ||
		!m_VisibilityOverlayPixelShader ||
		!m_VisibilityOverlayInputLayout ||
		!m_VisibilityOverlayConstantBuffer)
	{
		return;
	}

	ID3D11DeviceContext* context =
		GetDeviceContext();

	if (!context)
	{
		return;
	}

	//=================================================
	// Constant Buffer
	//=================================================

	VisibilityOverlayConstantBuffer
		constantBuffer{};

	const Matrix4x4 viewProjection =
		m_ViewMatrix *
		m_ProjectionMatrix;

	constantBuffer.World =
		worldMatrix.Transpose();

	constantBuffer.ViewProjection =
		viewProjection.Transpose();

	constantBuffer.Parameters =
		Vector4(
			cellSize,
			parchmentTiling,
			heightOffset,
			time);

	context->
		UpdateSubresource(
			m_VisibilityOverlayConstantBuffer.Get(),
			0,
			nullptr,
			&constantBuffer,
			0,
			0);

	//=================================================
	// Pipeline
	//=================================================

	context->
		IASetInputLayout(
			m_VisibilityOverlayInputLayout.Get());

	context->
		VSSetShader(
			m_VisibilityOverlayVertexShader.Get(),
			nullptr,
			0);

	context->
		PSSetShader(
			m_VisibilityOverlayPixelShader.Get(),
			nullptr,
			0);

	ID3D11Buffer*
		constantBuffers[] =
	{
		m_VisibilityOverlayConstantBuffer.Get()
	};

	context->
		VSSetConstantBuffers(
			0,
			1,
			constantBuffers);

	context->
		PSSetConstantBuffers(
			0,
			1,
			constantBuffers);

	//=================================================
	// Textures
	//=================================================

	ID3D11ShaderResourceView*
		srvs[2] =
	{
		// t0
		parchmentTexture->
			GetShaderResourceView(),

		// t1
		visibilitySRV
	};

	context->
		PSSetShaderResources(
			0,
			2,
			srvs);

	//=================================================
	// Samplers
	//=================================================

	ID3D11SamplerState*
		samplers[2] =
	{
		// s0 : Parchment Texture
		m_ModelSamplerState.Get(),

		// s1 : Visibility Texture
		m_VisibilitySampler.Get()
	};

	context->
		PSSetSamplers(
			0,
			2,
			samplers);

	//=================================================
	// Render State
	//=================================================

	SetDepthEnable(
		true);

	SetBlendState(
		BlendState::Alpha);

	SetCulling(
		true);

	//=================================================
	// Draw
	//=================================================

	mesh.Bind(
		context);

	mesh.Draw(
		context);
	SetBlendState(
		BlendState::None);

	//=================================================
	// Unbind
	//=================================================

	ID3D11ShaderResourceView*
		nullSRVs[2] =
	{
		nullptr,
		nullptr
	};

	context->
		PSSetShaderResources(
			0,
			2,
			nullSRVs);
}

//=====================================================
// Depth
//=====================================================

void Renderer::SetDepthEnable(
	bool enable)
{
	if (!m_DeviceContext)
	{
		return;
	}

	m_DeviceContext->
		OMSetDepthStencilState(
			enable
			? m_DepthStateEnable.Get()
			: m_DepthStateDisable.Get(),
			0);
}


//=====================================================
// Blend
//=====================================================

void Renderer::SetBlendState(
	BlendState state)
{
	if (!m_DeviceContext)
	{
		return;
	}

	const size_t index =
		static_cast<size_t>(state);

	if (index >=
		static_cast<size_t>(
			BlendState::Count))
	{
		return;
	}

	const float blendFactor[4] =
	{
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};

	m_DeviceContext->
		OMSetBlendState(
			m_BlendStates[index].Get(),
			blendFactor,
			0xffffffff);
}


//=====================================================
// Rasterizer
//=====================================================

void Renderer::SetCulling(
	bool enable)
{
	m_CullingEnabled =
		enable;

	SetFillMode(
		m_CurrentFillMode);
}


void Renderer::SetFillMode(
	D3D11_FILL_MODE fillMode)
{
	if (!m_DeviceContext)
	{
		return;
	}

	m_CurrentFillMode =
		fillMode;

	ID3D11RasterizerState*
		rasterizerState = nullptr;

	if (fillMode ==
		D3D11_FILL_WIREFRAME)
	{
		rasterizerState =
			m_CullingEnabled
			? m_RasterizerWireframeCull.Get()
			: m_RasterizerWireframeNoCull.Get();
	}
	else
	{
		rasterizerState =
			m_CullingEnabled
			? m_RasterizerSolidCull.Get()
			: m_RasterizerSolidNoCull.Get();
	}

	m_DeviceContext->
		RSSetState(
			rasterizerState);
}

//=====================================================
// Camera
//=====================================================

void Renderer::SetCamera(
	const Matrix4x4& viewMatrix,
	const Matrix4x4& projectionMatrix)
{
	m_ViewMatrix =
		viewMatrix;

	m_ProjectionMatrix =
		projectionMatrix;
}

void Renderer::SetDirectionalLight(
	const DirectionalLight& light)
{
	m_DirectionalLight =
		light;

	// Shader側では正規化済みの方向を使えるようにする
	m_DirectionalLight.Direction =
		NormalizeDirection(
			light.Direction);
}

void Renderer::SetAmbientLight(
	const Color& color)
{
	m_AmbientLight =
		color;
}

void Renderer::SetToonShading(
	const ToonShading& toon)
{
	m_ToonShading =
		toon;
}

void Renderer::SetOutlineSetting(
	const OutlineSetting& setting)
{
	m_OutlineSetting =
		setting;
}

void Renderer::DrawOutline(
	const MeshBuffer& mesh,
	const Matrix4x4& worldMatrix,
	std::span<const Matrix4x4> boneMatrices)
{
	if (!m_DeviceContext ||
		!mesh.IsValid())
	{
		return;
	}

	//=================================================
	// Outline ConstantBuffer
	//=================================================

	OutlineConstantBuffer
		constantBuffer{};

	constantBuffer.WorldViewProjection =
		(
			worldMatrix *
			m_ViewMatrix *
			m_ProjectionMatrix
			).Transpose();

	const Matrix4x4
		worldInverseTranspose =
		CreateWorldInverseTranspose(
			worldMatrix);

	constantBuffer.WorldInverseTranspose =
		worldInverseTranspose.Transpose();

	constantBuffer.OutlineParameters =
		Vector4(
			m_OutlineSetting.Width,
			0.0f,
			0.0f,
			0.0f);

	constantBuffer.OutlineColor =
		Vector4(
			m_OutlineSetting.OutlineColor.x,
			m_OutlineSetting.OutlineColor.y,
			m_OutlineSetting.OutlineColor.z,
			m_OutlineSetting.OutlineColor.w);

	m_DeviceContext->
		UpdateSubresource(
			m_OutlineConstantBuffer.Get(),
			0,
			nullptr,
			&constantBuffer,
			0,
			0);

	//=================================================
	// Bone ConstantBuffer
	//=================================================

	BoneConstantBuffer
		boneConstantBuffer{};

	if (!boneMatrices.empty())
	{
		if (boneMatrices.size() >
			MAX_BONES)
		{
			return;
		}

		boneConstantBuffer.HasSkinning =
			1;

		for (size_t i = 0;
			i < boneMatrices.size();
			++i)
		{
			boneConstantBuffer.
				BoneMatrices[i] =
				boneMatrices[i].
				Transpose();
		}
	}

	m_DeviceContext->
		UpdateSubresource(
			m_BoneConstantBuffer.Get(),
			0,
			nullptr,
			&boneConstantBuffer,
			0,
			0);

	//=================================================
	// InputAssembler
	//=================================================

	m_DeviceContext->
		IASetInputLayout(
			m_ModelInputLayout.Get());

	//=================================================
	// Shader
	//=================================================

	m_DeviceContext->
		VSSetShader(
			m_OutlineVertexShader.Get(),
			nullptr,
			0);

	m_DeviceContext->
		PSSetShader(
			m_OutlinePixelShader.Get(),
			nullptr,
			0);

	// VS b0
	ID3D11Buffer* outlineBuffers[] =
	{
		m_OutlineConstantBuffer.Get()
	};

	m_DeviceContext->
		VSSetConstantBuffers(
			0,
			1,
			outlineBuffers);

	// PS b0
	m_DeviceContext->
		PSSetConstantBuffers(
			0,
			1,
			outlineBuffers);

	// VS b1
	ID3D11Buffer* boneBuffers[] =
	{
		m_BoneConstantBuffer.Get()
	};

	m_DeviceContext->
		VSSetConstantBuffers(
			1,
			1,
			boneBuffers);

	//=================================================
	// Rasterizer
	//=================================================

	m_DeviceContext->
		RSSetState(
			m_OutlineRasterizerState.Get());

	//=================================================
	// Draw
	//=================================================

	mesh.Bind(
		m_DeviceContext.Get());

	mesh.Draw(
		m_DeviceContext.Get());
}

void Renderer::BeginShadowPass(
	const Vector3& focusPosition)
{
	if (!m_DeviceContext ||
		!m_ShadowMapDSV)
	{
		return;
	}

	//=================================================
	// Shadow SRV解除
	//=================================================
	//
	// 前FrameでPS t7にBindされているShadowMapを
	// DSVとして使用する前に解除する。
	//=================================================

	ID3D11ShaderResourceView*
		nullSRV =
		nullptr;

	m_DeviceContext->
		PSSetShaderResources(
			7,
			1,
			&nullSRV);

	//=================================================
	// Directional Light Camera
	//=================================================

	Vector3 direction =
		m_DirectionalLight.Direction;

	const float directionLengthSq =
		direction.x * direction.x +
		direction.y * direction.y +
		direction.z * direction.z;

	if (directionLengthSq <=
		0.000001f)
	{
		direction =
			Vector3(
				0.35f,
				-1.0f,
				0.30f);
	}
	else
	{
		direction.Normalize();
	}

	constexpr float LIGHT_DISTANCE =
		50.0f;

	const Vector3 lightPosition =
		focusPosition -
		direction *
		LIGHT_DISTANCE;

	// 通常はY-Up。
	// Lightがほぼ垂直の場合のみZ-Upへ逃がす。
	Vector3 up =
		Vector3(
			0.0f,
			1.0f,
			0.0f);

	const float verticalDot =
		std::abs(
			direction.y);

	if (verticalDot >
		0.98f)
	{
		up =
			Vector3(
				0.0f,
				0.0f,
				1.0f);
	}

	const Matrix4x4 lightView =
		Matrix4x4::CreateLookAt(
			lightPosition,
			focusPosition,
			up);

	//=================================================
	// Directional LightなのでOrthographic
	//=================================================

	constexpr float SHADOW_AREA =
		70.0f;

	constexpr float SHADOW_NEAR =
		1.0f;

	constexpr float SHADOW_FAR =
		120.0f;

	const Matrix4x4 lightProjection =
		Matrix4x4::CreateOrthographic(
			SHADOW_AREA,
			SHADOW_AREA,
			SHADOW_NEAR,
			SHADOW_FAR);

	m_LightViewProjectionMatrix =
		lightView *
		lightProjection;

	//=================================================
	// Shadow Receive CB
	//=================================================

	ShadowReceiveConstantBuffer
		receiveBuffer{};

	receiveBuffer.LightViewProjection =
		m_LightViewProjectionMatrix.
		Transpose();

	receiveBuffer.ShadowParameters =
		Vector4(
			0.0010f,	// Bias
			0.60f,		// Shadow Strength
			static_cast<float>(
				SHADOW_MAP_SIZE),
			0.0f);

	m_DeviceContext->
		UpdateSubresource(
			m_ShadowReceiveConstantBuffer.Get(),
			0,
			nullptr,
			&receiveBuffer,
			0,
			0);

	//=================================================
	// Shadow Viewport
	//=================================================

	D3D11_VIEWPORT viewport{};

	viewport.TopLeftX =
		0.0f;

	viewport.TopLeftY =
		0.0f;

	viewport.Width =
		static_cast<float>(
			SHADOW_MAP_SIZE);

	viewport.Height =
		static_cast<float>(
			SHADOW_MAP_SIZE);

	viewport.MinDepth =
		0.0f;

	viewport.MaxDepth =
		1.0f;

	m_DeviceContext->
		RSSetViewports(
			1,
			&viewport);

	//=================================================
	// Depth Only RenderTarget
	//=================================================

	m_DeviceContext->
		OMSetRenderTargets(
			0,
			nullptr,
			m_ShadowMapDSV.Get());

	m_DeviceContext->
		ClearDepthStencilView(
			m_ShadowMapDSV.Get(),
			D3D11_CLEAR_DEPTH,
			1.0f,
			0);

	SetDepthEnable(
		true);

	SetBlendState(
		BlendState::None);

	m_DeviceContext->
		RSSetState(
			m_ShadowRasterizerCull.Get());
}

void Renderer::EndShadowPass()
{
	if (!m_DeviceContext)
	{
		return;
	}

	//=================================================
	// Shadow DSV解除
	//=================================================

	m_DeviceContext->
		OMSetRenderTargets(
			0,
			nullptr,
			nullptr);

	//=================================================
	// Main RenderTarget復元
	//=================================================

	ID3D11RenderTargetView*
		renderTargets[] =
	{
		m_RenderTargetView.Get()
	};

	m_DeviceContext->
		OMSetRenderTargets(
			1,
			renderTargets,
			m_DepthStencilView.Get());

	//=================================================
	// Main Viewport復元
	//=================================================

	CreateViewport();

	//=================================================
	// Render State復元
	//=================================================

	SetDepthEnable(
		true);

	SetBlendState(
		BlendState::None);

	SetCulling(
		true);

	SetFillMode(
		D3D11_FILL_SOLID);

	//=================================================
	// Shadow Receive Resource Bind
	//=================================================

	ID3D11Buffer*
		shadowReceiveBuffer =
		m_ShadowReceiveConstantBuffer.Get();

	// ShadowはPS側で評価する
	m_DeviceContext->
		PSSetConstantBuffers(
			2,
			1,
			&shadowReceiveBuffer);

	ID3D11ShaderResourceView*
		shadowSRV =
		m_ShadowMapSRV.Get();

	m_DeviceContext->
		PSSetShaderResources(
			7,
			1,
			&shadowSRV);

	ID3D11SamplerState*
		shadowSampler =
		m_ShadowSampler.Get();

	m_DeviceContext->
		PSSetSamplers(
			7,
			1,
			&shadowSampler);
}

void Renderer::DrawShadowStatic(
	const MeshBuffer& mesh,
	const Matrix4x4& worldMatrix)
{
	if (!m_DeviceContext ||
		!mesh.IsValid() ||
		!m_ShadowStaticVertexShader ||
		!m_ShadowStaticInputLayout)
	{
		return;
	}

	//=================================================
	// Light WVP
	//=================================================

	ShadowCastConstantBuffer
		constantBuffer{};

	constantBuffer.
		LightWorldViewProjection =
		(
			worldMatrix *
			m_LightViewProjectionMatrix
			).Transpose();

	m_DeviceContext->
		UpdateSubresource(
			m_ShadowCastConstantBuffer.Get(),
			0,
			nullptr,
			&constantBuffer,
			0,
			0);

	//=================================================
	// Pipeline
	//=================================================

	m_DeviceContext->
		IASetInputLayout(
			m_ShadowStaticInputLayout.Get());

	m_DeviceContext->
		VSSetShader(
			m_ShadowStaticVertexShader.Get(),
			nullptr,
			0);

	// Depth Only
	m_DeviceContext->
		PSSetShader(
			nullptr,
			nullptr,
			0);

	ID3D11Buffer*
		constantBuffers[] =
	{
		m_ShadowCastConstantBuffer.Get()
	};

	m_DeviceContext->
		VSSetConstantBuffers(
			0,
			1,
			constantBuffers);

	m_DeviceContext->
		RSSetState(
			m_ShadowRasterizerCull.Get());

	mesh.Bind(
		m_DeviceContext.Get());

	mesh.Draw(
		m_DeviceContext.Get());
}

void Renderer::DrawShadowSkinned(
	const MeshBuffer& mesh,
	const Matrix4x4& worldMatrix,
	std::span<const Matrix4x4> boneMatrices)
{
	if (!m_DeviceContext ||
		!mesh.IsValid())
	{
		return;
	}

	if (boneMatrices.empty() ||
		boneMatrices.size() >
		MAX_BONES)
	{
		return;
	}

	//=================================================
	// Shadow Matrix
	//=================================================

	ShadowCastConstantBuffer
		shadowBuffer{};

	shadowBuffer.
		LightWorldViewProjection =
		(
			worldMatrix *
			m_LightViewProjectionMatrix
			).Transpose();

	m_DeviceContext->
		UpdateSubresource(
			m_ShadowCastConstantBuffer.Get(),
			0,
			nullptr,
			&shadowBuffer,
			0,
			0);

	//=================================================
	// Bone
	//=================================================

	BoneConstantBuffer
		boneBuffer{};

	boneBuffer.HasSkinning =
		1;

	for (size_t i = 0;
		i < boneMatrices.size();
		++i)
	{
		boneBuffer.BoneMatrices[i] =
			boneMatrices[i].
			Transpose();
	}

	m_DeviceContext->
		UpdateSubresource(
			m_BoneConstantBuffer.Get(),
			0,
			nullptr,
			&boneBuffer,
			0,
			0);

	//=================================================
	// Pipeline
	//=================================================

	m_DeviceContext->
		IASetInputLayout(
			m_ShadowSkinnedInputLayout.Get());

	m_DeviceContext->
		VSSetShader(
			m_ShadowSkinnedVertexShader.Get(),
			nullptr,
			0);

	m_DeviceContext->
		PSSetShader(
			nullptr,
			nullptr,
			0);

	ID3D11Buffer*
		shadowCB =
		m_ShadowCastConstantBuffer.Get();

	m_DeviceContext->
		VSSetConstantBuffers(
			0,
			1,
			&shadowCB);

	ID3D11Buffer*
		boneCB =
		m_BoneConstantBuffer.Get();

	m_DeviceContext->
		VSSetConstantBuffers(
			1,
			1,
			&boneCB);

	m_DeviceContext->
		RSSetState(
			m_ShadowRasterizerCull.Get());

	mesh.Bind(
		m_DeviceContext.Get());

	mesh.Draw(
		m_DeviceContext.Get());
}

void Renderer::DrawShadowGrassInstanced(
	const MeshBuffer& mesh,
	ID3D11Buffer* instanceBuffer,
	uint32_t instanceCount,
	const Texture* grassTexture,
	float time,
	const Vector2& windDirection,
	float windStrength,
	float windSpeed)
{
	if (!m_DeviceContext ||
		!mesh.IsValid() ||
		!instanceBuffer ||
		instanceCount == 0 ||
		!grassTexture ||
		!grassTexture->IsValid())
	{
		return;
	}

	//=================================================
	// Grass ConstantBuffer
	//=================================================
	//
	// 通常描画ではViewProjection。
	// Shadow描画ではLightViewProjectionを送る。
	//=================================================

	GrassConstantBuffer
		constantBuffer{};

	constantBuffer.ViewProjection =
		m_LightViewProjectionMatrix.
		Transpose();

	constantBuffer.GrassParameters =
		Vector4(
			time,
			windStrength,
			windSpeed,
			0.40f);

	constantBuffer.WindParameters =
		Vector4(
			windDirection.x,
			windDirection.y,
			0.0f,
			0.0f);

	m_DeviceContext->
		UpdateSubresource(
			m_GrassConstantBuffer.Get(),
			0,
			nullptr,
			&constantBuffer,
			0,
			0);

	//=================================================
	// Pipeline
	//=================================================

	m_DeviceContext->
		IASetInputLayout(
			m_ShadowGrassInputLayout.Get());

	m_DeviceContext->
		VSSetShader(
			m_ShadowGrassVertexShader.Get(),
			nullptr,
			0);

	// Alpha Clipが必要なのでPSを使用
	m_DeviceContext->
		PSSetShader(
			m_ShadowGrassPixelShader.Get(),
			nullptr,
			0);

	ID3D11Buffer*
		grassCB =
		m_GrassConstantBuffer.Get();

	m_DeviceContext->
		VSSetConstantBuffers(
			0,
			1,
			&grassCB);

	m_DeviceContext->
		PSSetConstantBuffers(
			0,
			1,
			&grassCB);

	//=================================================
	// Texture
	//=================================================

	ID3D11ShaderResourceView*
		grassSRV =
		grassTexture->
		GetShaderResourceView();

	m_DeviceContext->
		PSSetShaderResources(
			0,
			1,
			&grassSRV);

	ID3D11SamplerState*
		sampler =
		m_ModelSamplerState.Get();

	m_DeviceContext->
		PSSetSamplers(
			0,
			1,
			&sampler);

	//=================================================
	// Vertex Buffers
	//=================================================

	mesh.Bind(
		m_DeviceContext.Get());

	ID3D11Buffer*
		instanceBuffers[] =
	{
		instanceBuffer
	};

	const UINT strides[] =
	{
		sizeof(
			GrassInstanceData)
	};

	const UINT offsets[] =
	{
		0
	};

	m_DeviceContext->
		IASetVertexBuffers(
			1,
			1,
			instanceBuffers,
			strides,
			offsets);

	// Grassは両面
	m_DeviceContext->
		RSSetState(
			m_ShadowRasterizerNoCull.Get());

	m_DeviceContext->
		DrawIndexedInstanced(
			mesh.GetIndexCount(),
			instanceCount,
			0,
			0,
			0);

	// SRV解除
	ID3D11ShaderResourceView*
		nullSRV =
		nullptr;

	m_DeviceContext->
		PSSetShaderResources(
			0,
			1,
			&nullSRV);
}