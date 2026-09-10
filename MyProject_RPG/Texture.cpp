#include "Texture.h"

#include <limits>

//=====================================================
// stb_image
//=====================================================
// STB_IMAGE_IMPLEMENTATIONはプロジェクト内で
// 必ず1つのcppにだけ定義すること！
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * @brief 画像ファイルからTexture生成
 */
bool Texture::Load(
	ID3D11Device* device,
	const std::string& filePath)
{
	//====================
	// 引数確認
	//====================

	if (!device ||
		filePath.empty())
	{
		return false;
	}

	// 既存Textureがある場合は破棄
	Release();

	//====================
	// stb_image
	//====================

	int width = 0;
	int height = 0;
	int channels = 0;

	// RGBA8へ統一して読み込む。
	// GPU側のTexture Formatを統一できるため、
	// Renderer側の処理を簡潔にできる。
	stbi_uc* imageData =
		stbi_load(
			filePath.c_str(),
			&width,
			&height,
			&channels,
			STBI_rgb_alpha);

	if (!imageData)
	{
		return false;
	}

	//====================
	// サイズチェック
	//====================

	if (width <= 0 ||
		height <= 0)
	{
		stbi_image_free(imageData);
		return false;
	}

	// D3D11のUINTへ安全に変換できるか確認
	if (static_cast<uint64_t>(width) >
		(std::numeric_limits<UINT>::max)() ||
		static_cast<uint64_t>(height) >
		(std::numeric_limits<UINT>::max)())
	{
		stbi_image_free(imageData);
		return false;
	}

	//====================
	// Texture2D作成
	//====================

	D3D11_TEXTURE2D_DESC textureDesc{};

	textureDesc.Width =
		static_cast<UINT>(width);

	textureDesc.Height =
		static_cast<UINT>(height);

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	// Diffuse TextureなのでsRGBとして扱う。
	// NormalMap等を実装する時は別Formatを使用する。
	textureDesc.Format =
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage =
		D3D11_USAGE_DEFAULT;

	textureDesc.BindFlags =
		D3D11_BIND_SHADER_RESOURCE;

	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//====================
	// 初期データ
	//====================

	D3D11_SUBRESOURCE_DATA
		initialData{};

	initialData.pSysMem =
		imageData;

	// RGBA = 4byte
	initialData.SysMemPitch =
		static_cast<UINT>(
			width * 4);

	HRESULT hr =
		device->CreateTexture2D(
			&textureDesc,
			&initialData,
			m_Texture.GetAddressOf());

	// CPU画像はGPUへコピー済みなので解放
	stbi_image_free(imageData);

	if (FAILED(hr))
	{
		Release();
		return false;
	}

	//====================
	// ShaderResourceView
	//====================

	hr =
		device->CreateShaderResourceView(
			m_Texture.Get(),
			nullptr,
			m_ShaderResourceView.GetAddressOf());

	if (FAILED(hr))
	{
		Release();
		return false;
	}

	//====================
	// 情報保存
	//====================

	m_Width =
		static_cast<uint32_t>(width);

	m_Height =
		static_cast<uint32_t>(height);

	return true;
}

/**
 * @brief GPUリソース解放
 */
void Texture::Release()
{
	m_ShaderResourceView.Reset();
	m_Texture.Reset();

	m_Width = 0;
	m_Height = 0;
}