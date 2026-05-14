#include "InputCheckImGuiWindow.h"

#include "../../../../Application/main.h"
#include "../../../../Application/PlayerInputConfig/PlayerInputConfig.h"

#include "../../../../Application/Utility/ImGui/ImGuiUtility.h"
#include "../../../../Application/Utility/BitShift/BitShiftUtility.h"

#include "../../../Input/RawInputManager.h"

void InputCheckImGuiWindow::Update()
{
	if (ImGui::Begin("InputCheck"))
	{
		PressKeyInputCheck("KeyMoveRight" , CommonEnum::UseKeyInputFlags::KeyRight );
		PressKeyInputCheck("KeyMoveLeft"  , CommonEnum::UseKeyInputFlags::KeyLeft  );
		PressKeyInputCheck("KeyMoveUp"    , CommonEnum::UseKeyInputFlags::KeyUp    );
		PressKeyInputCheck("KeyMoveDown"  , CommonEnum::UseKeyInputFlags::KeyDown  );

		ClickMouseInputCheck("ClickMouseLeft"  , CommonEnum::UseMouseInputFlags::ClickMouseLeft  );
		ClickMouseInputCheck("ClickMouseMiddle", CommonEnum::UseMouseInputFlags::ClickMouseMiddle);
		ClickMouseInputCheck("ClickMouseRight" , CommonEnum::UseMouseInputFlags::ClickMouseRight );
	}
	ImGui::End();
}

void InputCheckImGuiWindow::PressKeyInputCheck(const char* Label, CommonEnum::UseKeyInputFlags UseKeyFlag)
{
	auto&       playerKeyConfig_ = Application::Instance().GetPlayerInputConfig();
	const auto& inputManager_    = RawInputManager::GetInstance                ();

	const int  virtualKeyCode_ = playerKeyConfig_.GetKeyConfigData(UseKeyFlag     );
	const bool isValid_        = inputManager_.GetKeyState     (virtualKeyCode_);

	const char* boolString_   = ImGuiUtility::BoolToString(isValid_);

	ImGui::Text("%s : %s" , Label , boolString_);
}
void InputCheckImGuiWindow::ClickMouseInputCheck(const char* Label, CommonEnum::UseMouseInputFlags UseKeyFlag)
{
	auto&       playerKeyConfig_ = Application::Instance().GetPlayerInputConfig();
	const auto& inputManager_    = RawInputManager::GetInstance                ();

	const int  virtualKeyCode_ = playerKeyConfig_.GetMouseConfigData(UseKeyFlag);
	const bool isValid_        = inputManager_.GetNowMouseInputState(virtualKeyCode_);

	const char* boolString_ = ImGuiUtility::BoolToString(isValid_);

	ImGui::Text("%s : %s" , Label , boolString_);
}