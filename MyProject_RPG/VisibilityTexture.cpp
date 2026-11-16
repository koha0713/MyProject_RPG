#include "VisibilityTexture.h"

#include <algorithm>

//=====================================================
// Initialize
//=====================================================

bool VisibilityTexture::Initialize(
	ID3D11Device* device,
	int width,
	int height)
{
	Finalize();

	if (!device)
	{
		return false;
	}

	if (width <= 0 ||
		height <= 0)
	{
		return false;
	}

	m_Width =
		width;

	m_Height =
		height;

	//=================================================
	// Texture
	//=================================================
	//
	// CPUから毎回更新するため
	// D3D11_USAGE_DYNAMICを使用する。
	//
	// 1GridにつきR8 1Pixelだけなので、
	// Texture容量は非常に小さい。
	//=================================================

	D3D11_TEXTURE2D_DESC textureDesc{};

	textureDesc.Width =
		static_cast<UINT>(
			width);

	textureDesc.Height =
		static_cast<UINT>(
			height);

	textureDesc.MipLevels =
		1;

	textureDesc.ArraySize =
		1;

	textureDesc.Format =
		DXGI_FORMAT_R8_UNORM;

	textureDesc.SampleDesc.Count =
		1;

	textureDesc.SampleDesc.Quality =
		0;

	textureDesc.Usage =
		D3D11_USAGE_DYNAMIC;

	textureDesc.BindFlags =
		D3D11_BIND_SHADER_RESOURCE;

	textureDesc.CPUAccessFlags =
		D3D11_CPU_ACCESS_WRITE;

	textureDesc.MiscFlags =
		0;

	const HRESULT textureResult =
		device->
		CreateTexture2D(
			&textureDesc,
			nullptr,
			m_Texture.
			GetAddressOf());

	if (FAILED(textureResult))
	{
		Finalize();

		return false;
	}

	//=================================================
	// Shader Resource View
	//=================================================

	D3D11_SHADER_RESOURCE_VIEW_DESC
		srvDesc{};

	srvDesc.Format =
		DXGI_FORMAT_R8_UNORM;

	srvDesc.ViewDimension =
		D3D11_SRV_DIMENSION_TEXTURE2D;

	srvDesc.Texture2D.MostDetailedMip =
		0;

	srvDesc.Texture2D.MipLevels =
		1;

	const HRESULT srvResult =
		device->
		CreateShaderResourceView(
			m_Texture.Get(),
			&srvDesc,
			m_ShaderResourceView.
			GetAddressOf());

	if (FAILED(srvResult))
	{
		Finalize();

		return false;
	}

	//=================================================
	// CPU Upload Buffer
	//=================================================

	const size_t pixelCount =
		static_cast<size_t>(
			width) *
		static_cast<size_t>(
			height);

	m_UploadBuffer.resize(
		pixelCount,
		0);

	return true;
}

//=====================================================
// Update
//=====================================================

bool VisibilityTexture::Update(
	ID3D11DeviceContext* context,
	const VisibilityMap& visibilityMap)
{
	if (!context ||
		!m_Texture)
	{
		return false;
	}

	if (visibilityMap.GetWidth() !=
		m_Width ||
		visibilityMap.GetHeight() !=
		m_Height)
	{
		return false;
	}

	const auto& states =
		visibilityMap.GetStates();

	if (states.size() !=
		m_UploadBuffer.size())
	{
		return false;
	}

	//=================================================
	// VisibilityState → R8
	//=================================================

	for (size_t i = 0;
		i < states.size();
		++i)
	{
		switch (states[i])
		{
		case VisibilityState::Unexplored:

			m_UploadBuffer[i] =
				0;

			break;

		case VisibilityState::Explored:

			m_UploadBuffer[i] =
				128;

			break;

		case VisibilityState::Visible:

			m_UploadBuffer[i] =
				255;

			break;

		default:

			m_UploadBuffer[i] =
				0;

			break;
		}
	}

	//=================================================
	// GPU Texture更新
	//=================================================

	D3D11_MAPPED_SUBRESOURCE
		mappedResource{};

	const HRESULT mapResult =
		context->
		Map(
			m_Texture.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource);

	if (FAILED(mapResult))
	{
		return false;
	}

	//=================================================
	// Row単位Copy
	//=================================================
	//
	// GPU側RowPitchは
	// widthと一致するとは限らないため、
	// 1行ずつコピーする。
	//=================================================

	const size_t sourceRowSize =
		static_cast<size_t>(
			m_Width);

	for (int y = 0;
		y < m_Height;
		++y)
	{
		uint8_t* destination =
			static_cast<uint8_t*>(
				mappedResource.pData) +
			static_cast<size_t>(
				y) *
			mappedResource.RowPitch;

		const uint8_t* source =
			m_UploadBuffer.data() +
			static_cast<size_t>(
				y) *
			sourceRowSize;

		std::copy_n(
			source,
			sourceRowSize,
			destination);
	}

	context->
		Unmap(
			m_Texture.Get(),
			0);

	return true;
}

//=====================================================
// Finalize
//=====================================================

void VisibilityTexture::Finalize()
{
	m_ShaderResourceView.Reset();

	m_Texture.Reset();

	m_UploadBuffer.clear();

	m_Width =
		0;

	m_Height =
		0;
}