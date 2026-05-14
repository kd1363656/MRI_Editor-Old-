#include "BTBlackBoardFinishedRandomMoveData.h"

#include "../../../../../Utility/ImGui/ImGuiUtility.h"

void BTBlackBoardFinishedRandomMoveData::ImGuiPrefabDataInspector()
{
	ImGui::Text("Bool : %s" , ImGuiUtility::BoolToString(Get()));
}