#include "Window.h"

//=========================
// ウィンドウネーム
//=========================
constexpr auto ClassName = TEXT("2026 GM31 ひな形");		// ウィンドウクラス名
constexpr auto WindowName = TEXT("2026 GM31 ひな形");	// ウィンドウ名

//=========================
// ウィンドウ生成
//=========================
bool Window::Create(
	int width,
	int height)
{
	m_width = width;
	m_height = height;

	// インスタンスハンドルを取得
	m_hInstance = GetModuleHandle(nullptr);
	if (m_hInstance == nullptr)
	{
		return false;
	}

	//======================
	// ウィンドウクラス登録
	//======================
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(m_hInstance,IDI_APPLICATION);
	wc.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(m_hInstance, IDI_APPLICATION);

	// ウィンドウを登録
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	//======================
	// ウィンドウサイズ設定
	//======================
	RECT rect = {};
	rect.right = static_cast<LONG>(m_width);
	rect.bottom = static_cast<LONG>(m_height);

	// ウィンドウサイズを調整.
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(
		&rect,
		style,
		FALSE);

	//======================
	// Window生成
	//======================
	m_hWnd = CreateWindowEx(
		0,
		ClassName,
		WindowName,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		m_hInstance,
		nullptr);

	// 生成失敗
	if (m_hWnd == nullptr)
	{
		return false;
	}

	//======================
	// Window表示
	//======================
	ShowWindow(m_hWnd, SW_SHOW);

	// ウィンドウ更新
	UpdateWindow(m_hWnd);

	// ウィンドウにフォーカス設定
	SetFocus(m_hWnd);

	return true;

}


//==============================
// メッセージ更新
//==============================
bool Window::ProcessMessage()
{
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// WM_QUITメッセージが来たら終了
		if (msg.message == WM_QUIT)
		{
			return false;
		}
	}

	return true;

}


//=========================
// ウィンドウプロシージャ
//=========================
LRESULT CALLBACK Window::WindowProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(
		hWnd,
		uMsg,
		wParam,
		lParam);
}


