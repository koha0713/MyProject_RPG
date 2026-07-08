#include "DebugUI.h"
#include "Window.h"

std::vector<std::function<void()>> DebugUI::m_debugUIFunctions;

void DebugUI::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // キーボードナビゲーションを有効化
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // ゲームパッドナビゲーションを有効化

	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();		// ダークテーマ
	// ImGui::StyleColorsLight();	// ライトテーマ


	io.Fonts->Clear(); // フォントをクリア

	// フォント設定
	ImFontConfig cfg;
	cfg.OversampleH = 2;
	cfg.OversampleV = 1;
	cfg.MergeMode = false;

	// 日本語フォントを追加
	io.Fonts->AddFontFromFileTTF(
		"C:\\Windows\\Fonts\\meiryo.ttc",
		18.0f,
		&cfg,
		io.Fonts->GetGlyphRangesJapanese()	// ここで日本語フォントを追加
	);

	io.Fonts->Build(); // フォントをビルド

	// DirectX11用のImGuiバックエンドを初期化
	ImGui_ImplWin32_Init(Window::GetHandle());
	ImGui_ImplDX11_Init(device, context);
}

void DebugUI::Finalize()
{
	// ImGuiの終了処理
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DebugUI::Render()
{
	// ImGuiのフレーム開始
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// デバッグUIの描画
	ImGui::Begin("Debug UI"); // デバッグUIウィンドウの開始
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		1000.0f / io.Framerate, io.Framerate);
	ImGui::End(); // デバッグUIウィンドウの終了

	// 登録されたデバッグUI関数を呼び出す
	for (const auto& func : m_debugUIFunctions)
	{
		func();
	}

	// ImGuiの描画
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void DebugUI::RegisterDebugFunction(const std::function<void()>& func)
{
	m_debugUIFunctions.push_back(std::move(func));
}