#include "BTNodeTaskBase.h"

#include "../../../../Utility/ImGui/ImGuiUtility.h"

void BTNodeTaskBase::ImGuiNodeInspector()
{
	ImNodes::PushColorStyle(ImNodesCol_TitleBar , ImGuiUtility::TASK_NODE_COLOR);
	
	const int selfID_ = static_cast<int>(GetGUID());

	ImNodes::BeginNode(selfID_);
	{
		ImGuiUtility::DrawNodeTitleBar(GetNodeName());
		ImGui::Text("GUID : %d", selfID_);

		// ノード状態の描画
		for (const auto& state_ : ImGuiUtility::NODE_STATE_LIST)
		{
			if (GetNodeState() != state_.type) { continue; }
			ImGui::Text("State : %s", state_.label);
		}

		ImGuiUtility::DrawNodeInputAttribute (GetGUID());
	}
	ImNodes::EndNode();

	ImNodes::PopColorStyle();
}