#include "../../Application/main.h"

#include "KdDebugGUI.h"

#include "../../Application/Utility/Common/CommonConstant.h"
#include "../../Application/Utility/Json/JsonUtility.h"

#include "../../System/FileSystem/FileSystem.h"
#include "../../System/Input/RawInputManager.h"
#include "../../System/ImGui/ImGuiManager.h"

KdDebugGUI::KdDebugGUI()
{}
KdDebugGUI::~KdDebugGUI()
{ 
	GuiRelease(); 
}

void KdDebugGUI::GuiInit()
{
	// 初期化済みなら動作させない
	if (m_uqLog) return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();
	// Setup Dear ImGui style

	// "ImGuiDocking"導入
	ImGuiIO& io_ = ImGui::GetIO();

	// ↓↓↓ "Docking & Viewport"を有効にする
	io_.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io_.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io_.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Optional

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings

	ImGui_ImplWin32_Init(Application::Instance().GetWindowHandle());
	ImGui_ImplDX11_Init(
		KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());

#include "imgui/ja_glyph_ranges.h"
	ImFontConfig config;
	config.MergeMode = true;
	io_.Fonts->AddFontDefault();
	// 日本語対応
	io_.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	m_uqLog = std::make_unique<ImGuiAppLog>();

	LoadDebugGuiSetting();

	ImGuiManager::GetInstance().Init();
}

void KdDebugGUI::GuiProcess()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	// "ImGui"ウィンドウを表示するかしないかを決める
	SwitchCanShowWindowFlag();

	if (!m_canShowDebugWindowFlag) { return; }

	//===========================================================
	// "ImGui"開始
	//===========================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//===========================================================
	// 以下にImGui描画処理を記述
	//===========================================================

	//===========================================================
	// ドッキングスペース設定
	//===========================================================
	ImGuiWindowFlags     windowFlags_ = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport_    = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos     (viewport_->WorkPos);
	ImGui::SetNextWindowSize    (viewport_->WorkSize);
	ImGui::SetNextWindowViewport(viewport_->ID);

	windowFlags_ |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding   , 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize , 0.0f);
	ImGui::Begin("DockSpace", nullptr, windowFlags_);
	ImGui::PopStyleVar(2);

	// DockSpaceの作成
	ImGuiID dockSpaceID_ = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockSpaceID_ , ImVec2(0.0f , 0.0f) , ImGuiDockNodeFlags_None);

	ImGui::End();


	ImGuiManager::GetInstance().Update();

		// ログウィンドウ
	m_uqLog->Draw("LogWindow");
	

	//=====================================================
	// ログ出力 ・・・ AddLog("～") で追加
	//=====================================================

//	m_uqLog->AddLog("hello world\n");

	//=====================================================
	// 別ソースファイルからログを出力する場合
	//=====================================================

//	KdDebugGUI::Instance().AddLog("TestLog\n");

	//===========================================================
	// ここより上にImGuiの描画はする事
	//===========================================================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// "ImGuiDocking"のマルチウィンドウ用処理
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// 描画、更新
		ImGui::UpdatePlatformWindows       ();
		ImGui::RenderPlatformWindowsDefault();

		// レンダーターゲットを戻す
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets
		(
			1,
			KdDirect3D::Instance().GetBackBuffer()->WorkRTViewAddress(),
			KdDirect3D::Instance().GetZBuffer()->WorkDSView()
		);
	}
}

void KdDebugGUI::AddLog(const char* fmt,...)
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	va_list args;
	va_start(args, fmt);
	m_uqLog->AddLog(fmt);
	va_end(args);
}

void KdDebugGUI::LoadDebugGuiSetting()
{
	auto json_ = FileSystem::LoadJsonFile(DEBUG_GUI_SETTING_FILE_PATH);

	if (json_.is_null()) { return; }

	m_canShowDebugWindowFlag = json_.value("CanShowDebugWindowFlag" , false);
}
void KdDebugGUI::SaveDebugGuiSetting()
{
	auto json_ = nlohmann::json();

	json_["CanShowDebugWindowFlag"] = m_canShowDebugWindowFlag;
	
	FileSystem::SaveJsonFile(json_ , DEBUG_GUI_SETTING_FILE_PATH);
}

void KdDebugGUI::GuiRelease()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImNodes::DestroyContext();

	SaveDebugGuiSetting();
}

void KdDebugGUI::SwitchCanShowWindowFlag()
{
	auto& input_ = RawInputManager::GetInstance();

	if (input_.GetKeyState(VK_CONTROL))
	{
		if (input_.IsKeyPressedOnce(VK_RETURN))
		{
			m_canShowDebugWindowFlag = m_canShowDebugWindowFlag ? false : true;
		}
	}
}