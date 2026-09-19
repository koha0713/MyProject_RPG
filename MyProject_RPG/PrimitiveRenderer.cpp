#include "PrimitiveRenderer.h"

#include <d3dcompiler.h>

#include "Renderer.h"

//=====================================================
// Constant Buffer
//=====================================================

namespace
{
	struct PrimitiveConstantBuffer
	{
		Matrix4x4 View;
		Matrix4x4 Projection;
	};

	/**
	 * @brief Shaderをコンパイルする
	 */
	bool CompilePrimitiveShader(
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
PrimitiveRenderer::m_VertexShader;

ComPtr<ID3D11PixelShader>
PrimitiveRenderer::m_PixelShader;

ComPtr<ID3D11InputLayout>
PrimitiveRenderer::m_InputLayout;

ComPtr<ID3D11Buffer>
PrimitiveRenderer::m_VertexBuffer;

ComPtr<ID3D11Buffer>
PrimitiveRenderer::m_ConstantBuffer;

size_t
PrimitiveRenderer::m_VertexCapacity =
0;

//=====================================================
// Lifecycle
//=====================================================

bool PrimitiveRenderer::Initialize()
{
	return CreatePipeline();
}

void PrimitiveRenderer::Finalize()
{
	m_ConstantBuffer.Reset();
	m_VertexBuffer.Reset();
	m_InputLayout.Reset();
	m_PixelShader.Reset();
	m_VertexShader.Reset();

	m_VertexCapacity =
		0;
}

//=====================================================
// Pipeline
//=====================================================

bool PrimitiveRenderer::CreatePipeline()
{
	ID3D11Device* device =
		Renderer::GetDevice();

	if (!device)
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

	if (!CompilePrimitiveShader(
		L"VS_Primitive.hlsl",
		"main",
		"vs_5_0",
		vertexShaderBlob))
	{
		return false;
	}

	if (!CompilePrimitiveShader(
		L"PS_Primitive.hlsl",
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
					LineVertex,
					Position)),
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
					LineVertex,
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
	// Constant Buffer
	//=================================================

	D3D11_BUFFER_DESC
		constantBufferDesc{};

	constantBufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(
				PrimitiveConstantBuffer));

	constantBufferDesc.Usage =
		D3D11_USAGE_DEFAULT;

	constantBufferDesc.BindFlags =
		D3D11_BIND_CONSTANT_BUFFER;

	hr =
		device->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			m_ConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//=====================================================
// Vertex Buffer
//=====================================================

bool PrimitiveRenderer::EnsureVertexBuffer(
	size_t vertexCount)
{
	if (vertexCount == 0)
	{
		return false;
	}

	// 現在のBufferで足りる。
	if (m_VertexBuffer &&
		m_VertexCapacity >=
		vertexCount)
	{
		return true;
	}

	ID3D11Device* device =
		Renderer::GetDevice();

	if (!device)
	{
		return false;
	}

	//=================================================
	// 少し余裕を持って確保
	//=================================================

	size_t newCapacity =
		64;

	while (newCapacity <
		vertexCount)
	{
		newCapacity *=
			2;
	}

	D3D11_BUFFER_DESC
		bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(LineVertex) *
			newCapacity);

	bufferDesc.Usage =
		D3D11_USAGE_DYNAMIC;

	bufferDesc.BindFlags =
		D3D11_BIND_VERTEX_BUFFER;

	bufferDesc.CPUAccessFlags =
		D3D11_CPU_ACCESS_WRITE;

	ComPtr<ID3D11Buffer>
		newBuffer;

	const HRESULT hr =
		device->CreateBuffer(
			&bufferDesc,
			nullptr,
			newBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	m_VertexBuffer =
		newBuffer;

	m_VertexCapacity =
		newCapacity;

	return true;
}

//=====================================================
// Draw Line
//=====================================================

void PrimitiveRenderer::DrawLine(
	const Vector3& start,
	const Vector3& end,
	const Color& color)
{
	std::vector<LineVertex>
		vertices;

	vertices.reserve(
		2);

	vertices.push_back(
		{
			start,
			color
		});

	vertices.push_back(
		{
			end,
			color
		});

	DrawLines(
		vertices);
}

//=====================================================
// Draw Lines
//=====================================================

void PrimitiveRenderer::DrawLines(
	const std::vector<LineVertex>& vertices)
{
	if (vertices.empty())
	{
		return;
	}

	// LineListなので偶数頂点が必要。
	if ((vertices.size() % 2) !=
		0)
	{
		return;
	}

	if (!EnsureVertexBuffer(
		vertices.size()))
	{
		return;
	}

	ID3D11DeviceContext* context =
		Renderer::GetDeviceContext();

	if (!context)
	{
		return;
	}

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
		vertices.data(),
		sizeof(LineVertex) *
		vertices.size());

	context->Unmap(
		m_VertexBuffer.Get(),
		0);

	//=================================================
	// Camera ConstantBuffer
	//=================================================

	PrimitiveConstantBuffer
		constantBuffer{};

	constantBuffer.View =
		Renderer::GetViewMatrix();

	constantBuffer.Projection =
		Renderer::GetProjectionMatrix();

	context->UpdateSubresource(
		m_ConstantBuffer.Get(),
		0,
		nullptr,
		&constantBuffer,
		0,
		0);

	//=================================================
	// Input Assembler
	//=================================================

	const UINT stride =
		sizeof(LineVertex);

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
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

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

	ID3D11Buffer* constantBuffers[] =
	{
		m_ConstantBuffer.Get()
	};

	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	//=================================================
	// Render State
	//=================================================

	Renderer::SetDepthEnable(
		true);

	Renderer::SetBlendState(
		BlendState::Alpha);

	Renderer::SetCulling(
		false);

	//=================================================
	// Draw
	//=================================================

	context->Draw(
		static_cast<UINT>(
			vertices.size()),
		0);

	//=================================================
	// 最低限Stateを戻す
	//=================================================

	Renderer::SetBlendState(
		BlendState::None);

	Renderer::SetCulling(
		true);
}

//=====================================================
// Draw Triangles
//=====================================================
void PrimitiveRenderer::DrawTriangles(
	const std::vector<LineVertex>& vertices)
{
	if (vertices.empty())
	{
		return;
	}

	// TriangleListなので3の倍数が必要
	if ((vertices.size() % 3) != 0)
	{
		return;
	}

	if (!EnsureVertexBuffer(
		vertices.size()))
	{
		return;
	}

	ID3D11DeviceContext* context =
		Renderer::GetDeviceContext();

	if (!context)
	{
		return;
	}

	//=================================================
	// Vertex転送
	//=================================================

	D3D11_MAPPED_SUBRESOURCE
		mappedResource{};

	const HRESULT hr =
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
		vertices.data(),
		sizeof(LineVertex) *
		vertices.size());

	context->Unmap(
		m_VertexBuffer.Get(),
		0);

	//=================================================
	// Camera
	//=================================================

	PrimitiveConstantBuffer
		constantBuffer{};

	constantBuffer.View =
		Renderer::GetViewMatrix();

	constantBuffer.Projection =
		Renderer::GetProjectionMatrix();

	context->UpdateSubresource(
		m_ConstantBuffer.Get(),
		0,
		nullptr,
		&constantBuffer,
		0,
		0);

	//=================================================
	// IA
	//=================================================

	const UINT stride =
		sizeof(LineVertex);

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

	ID3D11Buffer* constantBuffers[] =
	{
		m_ConstantBuffer.Get()
	};

	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffers);

	//=================================================
	// State
	//=================================================

	Renderer::SetDepthEnable(
		true);

	Renderer::SetBlendState(
		BlendState::Alpha);

	Renderer::SetCulling(
		false);

	//=================================================
	// Draw
	//=================================================

	context->Draw(
		static_cast<UINT>(
			vertices.size()),
		0);

	//=================================================
	// Restore
	//=================================================

	Renderer::SetBlendState(
		BlendState::None);

	Renderer::SetCulling(
		true);
}