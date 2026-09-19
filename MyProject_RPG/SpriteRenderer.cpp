/**
 * @file SpriteRenderer.cpp
 */

#include "SpriteRenderer.h"

#include <cstring>
#include <d3dcompiler.h>

#include "Renderer.h"
#include "Window.h"

namespace
{
	/**
	 * @brief Sprite描画用頂点
	 */
	struct SpriteVertex
	{
		Vector3 Position;

		Vector2 TexCoord;

		Color VertexColor;
	};

	/**
	 * @brief Shader Compile
	 */
	bool CompileSpriteShader(
		const wchar_t* filePath,
		const char* entryPoint,
		const char* shaderModel,
		ComPtr<ID3DBlob>& outBlob)
	{
		ComPtr<ID3DBlob>
			errorBlob;

		const HRESULT hr =
			D3DCompileFromFile(
				filePath,
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				entryPoint,
				shaderModel,
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				outBlob.GetAddressOf(),
				errorBlob.GetAddressOf());

		if (FAILED(hr))
		{
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
// Static Member
//=====================================================

ComPtr<ID3D11VertexShader>
SpriteRenderer::m_VertexShader;

ComPtr<ID3D11PixelShader>
SpriteRenderer::m_PixelShader;

ComPtr<ID3D11InputLayout>
SpriteRenderer::m_InputLayout;

ComPtr<ID3D11Buffer>
SpriteRenderer::m_VertexBuffer;

ComPtr<ID3D11SamplerState>
SpriteRenderer::m_SamplerState;

bool
SpriteRenderer::m_IsDrawing =
false;

//=====================================================
// Initialize
//=====================================================

bool SpriteRenderer::Initialize()
{
	return CreatePipeline();
}

//=====================================================
// Finalize
//=====================================================

void SpriteRenderer::Finalize()
{
	m_IsDrawing =
		false;

	m_SamplerState.Reset();

	m_VertexBuffer.Reset();

	m_InputLayout.Reset();

	m_PixelShader.Reset();

	m_VertexShader.Reset();
}

//=====================================================
// CreatePipeline
//=====================================================

bool SpriteRenderer::CreatePipeline()
{
	ID3D11Device* device =
		Renderer::GetDevice();

	if (!device)
	{
		return false;
	}

	//=================================================
	// Shader
	//=================================================

	ComPtr<ID3DBlob>
		vertexShaderBlob;

	ComPtr<ID3DBlob>
		pixelShaderBlob;

	if (!CompileSpriteShader(
		L"VS_Sprite.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	if (!CompileSpriteShader(
		L"PS_Sprite.hlsl",
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
		device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			nullptr,
			m_VertexShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Pixel Shader
	//=================================================

	hr =
		device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize(),
			nullptr,
			m_PixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Input Layout
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
					SpriteVertex,
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
					SpriteVertex,
					TexCoord)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			static_cast<UINT>(
				offsetof(
					SpriteVertex,
					VertexColor)),
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	hr =
		device->CreateInputLayout(
			inputElements,
			static_cast<UINT>(
				std::size(
					inputElements)),
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			m_InputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Dynamic Vertex Buffer
	//=================================================

	D3D11_BUFFER_DESC
		vertexBufferDesc{};

	vertexBufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(SpriteVertex) *
			6);

	vertexBufferDesc.Usage =
		D3D11_USAGE_DYNAMIC;

	vertexBufferDesc.BindFlags =
		D3D11_BIND_VERTEX_BUFFER;

	vertexBufferDesc.CPUAccessFlags =
		D3D11_CPU_ACCESS_WRITE;

	hr =
		device->CreateBuffer(
			&vertexBufferDesc,
			nullptr,
			m_VertexBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//=================================================
	// Sampler
	//=================================================

	D3D11_SAMPLER_DESC
		samplerDesc{};

	samplerDesc.Filter =
		D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	samplerDesc.AddressU =
		D3D11_TEXTURE_ADDRESS_CLAMP;

	samplerDesc.AddressV =
		D3D11_TEXTURE_ADDRESS_CLAMP;

	samplerDesc.AddressW =
		D3D11_TEXTURE_ADDRESS_CLAMP;

	samplerDesc.MinLOD =
		0.0f;

	samplerDesc.MaxLOD =
		D3D11_FLOAT32_MAX;

	hr =
		device->CreateSamplerState(
			&samplerDesc,
			m_SamplerState.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//=====================================================
// Begin
//=====================================================

void SpriteRenderer::Begin()
{
	if (m_IsDrawing)
	{
		return;
	}

	// UIなのでDepthTest不要
	Renderer::SetDepthEnable(
		false);

	// PNG Alphaを使用する
	Renderer::SetBlendState(
		BlendState::Alpha);

	Renderer::SetCulling(
		false);

	m_IsDrawing =
		true;
}

//=====================================================
// End
//=====================================================

void SpriteRenderer::End()
{
	if (!m_IsDrawing)
	{
		return;
	}

	// 通常3D描画向けへ戻す
	Renderer::SetDepthEnable(
		true);

	Renderer::SetBlendState(
		BlendState::None);

	Renderer::SetCulling(
		true);

	m_IsDrawing =
		false;
}

//=====================================================
// Draw
//=====================================================

void SpriteRenderer::Draw(
	ID3D11ShaderResourceView* texture,
	const Vector2& position,
	const Vector2& size,
	const Color& color)
{
	if (!m_IsDrawing ||
		!texture)
	{
		return;
	}

	if (size.x <= 0.0f ||
		size.y <= 0.0f)
	{
		return;
	}

	ID3D11DeviceContext* context =
		Renderer::GetDeviceContext();

	if (!context)
	{
		return;
	}

	const float screenWidth =
		static_cast<float>(
			Window::GetWidth());

	const float screenHeight =
		static_cast<float>(
			Window::GetHeight());

	if (screenWidth <= 0.0f ||
		screenHeight <= 0.0f)
	{
		return;
	}

	//=================================================
	// Screen座標 -> NDC
	//
	// Screen:
	// 左上 (0,0)
	//
	// NDC:
	// 左上 (-1,+1)
	// 右下 (+1,-1)
	//=================================================

	const float left =
		(position.x /
			screenWidth) *
		2.0f -
		1.0f;

	const float right =
		((position.x + size.x) /
			screenWidth) *
		2.0f -
		1.0f;

	const float top =
		1.0f -
		(position.y /
			screenHeight) *
		2.0f;

	const float bottom =
		1.0f -
		((position.y + size.y) /
			screenHeight) *
		2.0f;

	//=================================================
	// Quad
	//=================================================

	const SpriteVertex vertices[6] =
	{
		// Triangle 1
		{
			Vector3(left, top, 0.0f),
			Vector2(0.0f, 0.0f),
			color
		},

		{
			Vector3(right, top, 0.0f),
			Vector2(1.0f, 0.0f),
			color
		},

		{
			Vector3(left, bottom, 0.0f),
			Vector2(0.0f, 1.0f),
			color
		},

		// Triangle 2
		{
			Vector3(left, bottom, 0.0f),
			Vector2(0.0f, 1.0f),
			color
		},

		{
			Vector3(right, top, 0.0f),
			Vector2(1.0f, 0.0f),
			color
		},

		{
			Vector3(right, bottom, 0.0f),
			Vector2(1.0f, 1.0f),
			color
		}
	};

	//=================================================
	// Vertex転送
	//=================================================

	D3D11_MAPPED_SUBRESOURCE
		mappedResource{};

	HRESULT hr =
		context->Map(
			m_VertexBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource);

	if (FAILED(hr))
	{
		return;
	}

	memcpy(
		mappedResource.pData,
		vertices,
		sizeof(vertices));

	context->Unmap(
		m_VertexBuffer.Get(),
		0);

	//=================================================
	// IA
	//=================================================

	const UINT stride =
		sizeof(SpriteVertex);

	const UINT offset =
		0;

	ID3D11Buffer* vertexBuffer =
		m_VertexBuffer.Get();

	context->IASetVertexBuffers(
		0,
		1,
		&vertexBuffer,
		&stride,
		&offset);

	context->IASetInputLayout(
		m_InputLayout.Get());

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//=================================================
	// Shader
	//=================================================

	context->VSSetShader(
		m_VertexShader.Get(),
		nullptr,
		0);

	context->PSSetShader(
		m_PixelShader.Get(),
		nullptr,
		0);

	//=================================================
	// Texture
	//=================================================

	ID3D11ShaderResourceView*
		shaderResourceViews[] =
	{
		texture
	};

	context->PSSetShaderResources(
		0,
		1,
		shaderResourceViews);

	ID3D11SamplerState*
		samplers[] =
	{
		m_SamplerState.Get()
	};

	context->PSSetSamplers(
		0,
		1,
		samplers);

	//=================================================
	// Draw
	//=================================================

	context->Draw(
		6,
		0);
}