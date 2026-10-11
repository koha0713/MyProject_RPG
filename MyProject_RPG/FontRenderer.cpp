/**
 * @file FontRenderer.cpp
 */

#include "FontRenderer.h"

#include "Renderer.h"

#include <dxgi.h>

using Microsoft::WRL::ComPtr;

//=====================================================
// Utility
//=====================================================

namespace
{
	/**
	 * @brief HRESULTをOutputへ表示する
	 */
	void OutputHRESULT(
		const char* message,
		HRESULT hr)
	{
		char buffer[256]{};

		sprintf_s(
			buffer,
			"%s HRESULT=0x%08X\n",
			message,
			static_cast<unsigned int>(
				hr));

		OutputDebugStringA(
			buffer);
	}
}

//=====================================================
// Static Member
//=====================================================

ComPtr<ID2D1Factory1>
FontRenderer::m_D2DFactory;

ComPtr<ID2D1Device>
FontRenderer::m_D2DDevice;

ComPtr<ID2D1DeviceContext>
FontRenderer::m_D2DContext;

ComPtr<ID2D1Bitmap1>
FontRenderer::m_TargetBitmap;

ComPtr<ID2D1SolidColorBrush>
FontRenderer::m_Brush;

ComPtr<IDWriteFactory>
FontRenderer::m_DWriteFactory;

ComPtr<IDWriteFactory5>
FontRenderer::m_DWriteFactory5;

ComPtr<IDWriteFontCollection1>
FontRenderer::m_FontCollection;

std::wstring
FontRenderer::m_FontFamilyName;

bool
FontRenderer::m_IsDrawing =
false;

//=====================================================
// Initialize
//=====================================================

bool FontRenderer::Initialize(
	const std::wstring& fontPath)
{
	//=================================================
	// Renderer確認
	//=================================================

	ID3D11Device* d3dDevice =
		Renderer::GetDevice();

	IDXGISwapChain* swapChain =
		Renderer::GetSwapChain();

	if (!d3dDevice ||
		!swapChain)
	{
		OutputDebugStringA(
			"[FontRenderer] "
			"Renderer is not initialized.\n");

		return false;
	}

	//=================================================
	// Direct2D Factory
	//=================================================

	D2D1_FACTORY_OPTIONS
		factoryOptions{};

	factoryOptions.debugLevel =
		D2D1_DEBUG_LEVEL_NONE;

	HRESULT hr =
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory1),
			&factoryOptions,
			reinterpret_cast<void**>(
				m_D2DFactory.GetAddressOf()));

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"D2D1CreateFactory Failed.",
			hr);

		return false;
	}

	//=================================================
	// DirectWrite Factory
	//=================================================

	hr =
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(
				m_DWriteFactory.GetAddressOf()));

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"DWriteCreateFactory Failed.",
			hr);

		return false;
	}

	//=================================================
	// IDWriteFactory5取得
	//=================================================
	//
	// Custom Font Setを扱うため、
	// 新しいDirectWrite Interfaceを取得する。
	//=================================================

	hr =
		m_DWriteFactory.As(
			&m_DWriteFactory5);

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"IDWriteFactory5 Failed.",
			hr);

		return false;
	}

	//=================================================
	// Custom Font生成
	//=================================================

	if (!CreateCustomFont(
		fontPath))
	{
		return false;
	}

	//=================================================
	// DXGI Device取得
	//=================================================

	ComPtr<IDXGIDevice>
		dxgiDevice;

	hr =
		d3dDevice->QueryInterface(
			IID_PPV_ARGS(
				dxgiDevice.GetAddressOf()));

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"Query IDXGIDevice Failed.",
			hr);

		return false;
	}

	//=================================================
	// Direct2D Device
	//=================================================

	hr =
		m_D2DFactory->CreateDevice(
			dxgiDevice.Get(),
			m_D2DDevice.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"Create D2D Device Failed.",
			hr);

		return false;
	}

	//=================================================
	// Direct2D DeviceContext
	//=================================================

	hr =
		m_D2DDevice->
		CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			m_D2DContext.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"Create D2D Context Failed.",
			hr);

		return false;
	}

	//=================================================
	// SwapChain BackBuffer
	//=================================================

	ComPtr<IDXGISurface>
		backBufferSurface;

	hr =
		swapChain->GetBuffer(
			0,
			IID_PPV_ARGS(
				backBufferSurface.GetAddressOf()));

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"Get BackBuffer Failed.",
			hr);

		return false;
	}

	//=================================================
	// Direct2D Target Bitmap
	//=================================================

	const D2D1_BITMAP_PROPERTIES1
		bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET |
			D2D1_BITMAP_OPTIONS_CANNOT_DRAW,

			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE));

	hr =
		m_D2DContext->
		CreateBitmapFromDxgiSurface(
			backBufferSurface.Get(),
			&bitmapProperties,
			m_TargetBitmap.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"CreateBitmapFromDxgiSurface Failed.",
			hr);

		return false;
	}

	m_D2DContext->SetTarget(
		m_TargetBitmap.Get());

	//=================================================
	// Brush
	//=================================================

	hr =
		m_D2DContext->
		CreateSolidColorBrush(
			D2D1::ColorF(
				1.0f,
				1.0f,
				1.0f,
				1.0f),
			m_Brush.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"Create Brush Failed.",
			hr);

		return false;
	}

	m_IsDrawing =
		false;

	OutputDebugStringA(
		"[FontRenderer] "
		"Initialize Success.\n");

	return true;
}

//=====================================================
// CreateCustomFont
//=====================================================

bool FontRenderer::CreateCustomFont(
	const std::wstring& fontPath)
{
	if (!m_DWriteFactory ||
		!m_DWriteFactory5)
	{
		return false;
	}

	//=================================================
	// FontSetBuilder生成
	//=================================================

	ComPtr<IDWriteFontSetBuilder1>
		fontSetBuilder;

	HRESULT hr =
		m_DWriteFactory5->
		CreateFontSetBuilder(
			fontSetBuilder.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"CreateFontSetBuilder Failed.",
			hr);

		return false;
	}

	//=================================================
	// FontFile生成
	//=================================================
	//
	// WindowsへInstallせず、
	// 指定Fontファイルを直接参照する。
	//=================================================

	ComPtr<IDWriteFontFile>
		fontFile;

	hr =
		m_DWriteFactory->
		CreateFontFileReference(
			fontPath.c_str(),
			nullptr,
			fontFile.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"CreateFontFileReference Failed.",
			hr);

		return false;
	}

	//=================================================
	// FontSetへFontFile追加
	//=================================================

	hr =
		fontSetBuilder->AddFontFile(
			fontFile.Get());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"AddFontFile Failed.",
			hr);

		return false;
	}

	//=================================================
	// FontSet生成
	//=================================================

	ComPtr<IDWriteFontSet>
		fontSet;

	hr =
		fontSetBuilder->CreateFontSet(
			fontSet.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"CreateFontSet Failed.",
			hr);

		return false;
	}

	//=================================================
	// FontCollection生成
	//=================================================
	//
	// FontSetをCreateTextFormatから使用可能な
	// FontCollectionへ変換する。
	//=================================================

	ComPtr<IDWriteFactory3>
		factory3;

	hr =
		m_DWriteFactory.As(
			&factory3);

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"IDWriteFactory3 Failed.",
			hr);

		return false;
	}

	hr =
		factory3->
		CreateFontCollectionFromFontSet(
			fontSet.Get(),
			m_FontCollection.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"CreateFontCollectionFromFontSet Failed.",
			hr);

		return false;
	}

	//=================================================
	// Font Family名取得
	//=================================================

	if (!AcquireFontFamilyName())
	{
		return false;
	}

	return true;
}

//=====================================================
// AcquireFontFamilyName
//=====================================================

bool FontRenderer::AcquireFontFamilyName()
{
	if (!m_FontCollection)
	{
		return false;
	}

	//=================================================
	// FontFamily数確認
	//=================================================

	const UINT32 familyCount =
		m_FontCollection->
		GetFontFamilyCount();

	if (familyCount == 0)
	{
		OutputDebugStringA(
			"[FontRenderer] "
			"No font family found.\n");

		return false;
	}

	// Fontは1種類だけ使用するため、
	// 先頭Familyを使用する。
	ComPtr<IDWriteFontFamily>
		fontFamily;

	HRESULT hr =
		m_FontCollection->
		GetFontFamily(
			0,
			fontFamily.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"GetFontFamily Failed.",
			hr);

		return false;
	}

	//=================================================
	// Family Name取得
	//=================================================

	ComPtr<IDWriteLocalizedStrings>
		familyNames;

	hr =
		fontFamily->
		GetFamilyNames(
			familyNames.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"GetFamilyNames Failed.",
			hr);

		return false;
	}

	//=================================================
	// 使用するLocale決定
	//=================================================

	UINT32 nameIndex =
		0;

	BOOL exists =
		FALSE;

	hr =
		familyNames->
		FindLocaleName(
			L"ja-jp",
			&nameIndex,
			&exists);

	if (FAILED(hr) ||
		!exists)
	{
		// 日本語名が無ければ
		// 最初の名前を使用する。
		nameIndex =
			0;
	}

	//=================================================
	// Family Name長取得
	//=================================================

	UINT32 nameLength =
		0;

	hr =
		familyNames->
		GetStringLength(
			nameIndex,
			&nameLength);

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"GetStringLength Failed.",
			hr);

		return false;
	}

	// null文字分も確保する。
	std::wstring familyName(
		nameLength + 1,
		L'\0');

	hr =
		familyNames->
		GetString(
			nameIndex,
			familyName.data(),
			nameLength + 1);

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"GetString Failed.",
			hr);

		return false;
	}

	// std::wstring末尾の余分なnullを削除。
	familyName.resize(
		nameLength);

	m_FontFamilyName =
		familyName;

	OutputDebugStringW(
		L"[FontRenderer] FontFamily = ");

	OutputDebugStringW(
		m_FontFamilyName.c_str());

	OutputDebugStringW(
		L"\n");

	return true;
}

//=====================================================
// Finalize
//=====================================================

void FontRenderer::Finalize()
{
	m_IsDrawing =
		false;

	m_Brush.Reset();

	if (m_D2DContext)
	{
		m_D2DContext->SetTarget(
			nullptr);
	}

	m_TargetBitmap.Reset();

	m_D2DContext.Reset();

	m_D2DDevice.Reset();

	m_FontCollection.Reset();

	m_DWriteFactory5.Reset();

	m_DWriteFactory.Reset();

	m_D2DFactory.Reset();

	m_FontFamilyName.clear();
}

//=====================================================
// Begin
//=====================================================

void FontRenderer::Begin()
{
	if (!m_D2DContext)
	{
		return;
	}

	if (m_IsDrawing)
	{
		return;
	}

	m_D2DContext->BeginDraw();

	m_IsDrawing =
		true;
}

//=====================================================
// End
//=====================================================

void FontRenderer::End()
{
	if (!m_D2DContext)
	{
		return;
	}

	if (!m_IsDrawing)
	{
		return;
	}

	const HRESULT hr =
		m_D2DContext->EndDraw();

	m_IsDrawing =
		false;

	if (hr ==
		D2DERR_RECREATE_TARGET)
	{
		OutputDebugStringA(
			"[FontRenderer] "
			"D2D Target needs recreation.\n");
	}
	else if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"EndDraw Failed.",
			hr);
	}
}

//=====================================================
// DrawText
//=====================================================

void FontRenderer::DrawText(
	const std::wstring& text,
	float fontSize,
	const Vector2& position,
	const Vector2& size,
	const Color& color)
{
	//=================================================
	// Validation
	//=================================================

	if (!m_IsDrawing)
	{
		return;
	}

	if (!m_DWriteFactory ||
		!m_FontCollection ||
		!m_D2DContext ||
		!m_Brush)
	{
		return;
	}

	if (text.empty())
	{
		return;
	}

	if (fontSize <=
		0.0f)
	{
		return;
	}

	if (size.x <=
		0.0f ||
		size.y <=
		0.0f)
	{
		return;
	}

	//=================================================
	// TextFormat
	//=================================================
	//
	// FontはFontRendererが保持する
	// Custom Font Collectionから取得する。
	//
	// 現段階では毎Draw生成。
	// UI数が増えた段階でCache可能。
	//=================================================

	ComPtr<IDWriteTextFormat>
		textFormat;

	HRESULT hr =
		m_DWriteFactory->
		CreateTextFormat(
			m_FontFamilyName.c_str(),

			// Custom Font Collection
			m_FontCollection.Get(),

			DWRITE_FONT_WEIGHT_NORMAL,

			DWRITE_FONT_STYLE_NORMAL,

			DWRITE_FONT_STRETCH_NORMAL,

			fontSize,

			L"ja-jp",

			textFormat.GetAddressOf());

	if (FAILED(hr))
	{
		OutputHRESULT(
			"[FontRenderer] "
			"CreateTextFormat Failed.",
			hr);

		return;
	}

	//=================================================
	// Text Layout設定
	//=================================================

	textFormat->
		SetTextAlignment(
			DWRITE_TEXT_ALIGNMENT_LEADING);

	textFormat->
		SetParagraphAlignment(
			DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	textFormat->
		SetWordWrapping(
			DWRITE_WORD_WRAPPING_WRAP);

	//=================================================
	// Color
	//=================================================

	m_Brush->SetColor(
		D2D1::ColorF(
			color.x,
			color.y,
			color.z,
			color.w));

	//=================================================
	// Draw Area
	//=================================================

	const D2D1_RECT_F
		drawRect =
		D2D1::RectF(
			position.x,
			position.y,
			position.x +
			size.x,
			position.y +
			size.y);

	//=================================================
	// Draw
	//=================================================

	m_D2DContext->
		DrawTextA(
			text.c_str(),

			static_cast<UINT32>(
				text.length()),

			textFormat.Get(),

			drawRect,

			m_Brush.Get(),

			D2D1_DRAW_TEXT_OPTIONS_NONE,

			DWRITE_MEASURING_MODE_NATURAL);
}