/**
 * @file Renderer.cpp
 * @brief DirectX11 Renderer実装
 */

#include "Renderer.h"

#include <cstddef>
#include <d3dcompiler.h>

#include "Window.h"
#include "MeshBuffer.h"
#include "ModelData.h"

 //=====================================================
 // 内部定義
 //=====================================================

namespace
{
	/**
	 * @brief モデル描画用ConstantBuffer
	 *
	 * @details
	 * HLSL側のModelConstantBufferと
	 * メモリレイアウトを一致させる必要がある。
	 *
	 * ConstantBufferは16byte単位である必要がある。
	 *
	 * Matrix4x4 : 64byte
	 * Vector4   : 16byte
	 * 合計      : 80byte
	 */
	struct ModelConstantBuffer
	{
		Matrix4x4 WorldViewProjection;

		Vector4 DiffuseColor;

		// Textureが設定されているか
		uint32_t HasTexture = 0;

		// HLSL ConstantBufferは
		// 16byte境界へ合わせる必要があるためPadding
		float Padding[3]{};
	};

	static_assert(
		sizeof(ModelConstantBuffer) % 16 == 0,
		"ConstantBuffer size must be 16-byte aligned.");

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


//=====================================================
// Initialize
//=====================================================

bool Renderer::Init()
{
	//====================
	// Device
	//====================

	if (!CreateDevice())
	{
		return false;
	}

	//====================
	// RenderTarget
	//====================

	if (!CreateRenderTarget())
	{
		return false;
	}

	//====================
	// DepthBuffer
	//====================

	if (!CreateDepthStencil())
	{
		return false;
	}

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

	CreateViewport();

	//====================
	// RenderState
	//====================

	if (!CreateRenderStates())
	{
		return false;
	}

	//====================
	// Model描画Pipeline
	//====================

	if (!CreateModelPipeline())
	{
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

	m_ModelConstantBuffer.Reset();
	m_ModelInputLayout.Reset();

	m_ModelPixelShader.Reset();
	m_ModelVertexShader.Reset();

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
		0.10f,
		0.18f,
		0.25f,
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

	desc.BufferDesc.Format =
		DXGI_FORMAT_R8G8B8A8_UNORM;

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
			D3D11_RASTERIZER_DESC desc{};

			desc.FillMode = fillMode;

			desc.CullMode = cullMode;

			desc.FrontCounterClockwise =
				FALSE;

			desc.DepthClipEnable =
				TRUE;

			const HRESULT resultHr =
				Renderer::m_Device->
				CreateRasterizerState(
					&desc,
					result.GetAddressOf());

			return SUCCEEDED(resultHr);
		};

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


//=====================================================
// Model描画
//=====================================================

void Renderer::DrawMesh(
	const MeshBuffer& mesh,
	const MaterialData& material,
	const Matrix4x4& worldMatrix)
{
	if (!m_DeviceContext ||
		!mesh.IsValid())
	{
		return;
	}

	//=================================================
	// Camera
	//=================================================
	

	//=================================================
	// ConstantBuffer
	//=================================================

	ModelConstantBuffer
		constantBuffer{};

	// HLSLへ渡すためTransposeする
	constantBuffer.WorldViewProjection =
		(
			worldMatrix *
			m_ViewMatrix *
			m_ProjectionMatrix
			).Transpose();

	constantBuffer.DiffuseColor =
		material.Diffuse;

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