#include "GUIDImGuiWindow.h"

#include "../../../../Application/Scene/SceneManager.h"
#include "../../../../Application/Scene/BaseScene/BaseScene.h"

void GUIDImGuiWindow::Update()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto gameObjectGUIDManager_ = scene_->GetGUIDGameObjectManager().lock();
	if (!gameObjectGUIDManager_) { return; }

	auto behaviorTreeGUIDManager_ = scene_->GetGUIDBehaviorTreeManager().lock();
	if (!behaviorTreeGUIDManager_) { return; }

	if (ImGui::Begin("GUIDList"))
	{
		if (ImGui::TreeNode("GameObjectGUID"))
		{
			for (const auto& list_ : gameObjectGUIDManager_->GetUsedGUIDList())
			{
				ImGui::Text("ID : %d", list_);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("BehaviorTreeGUID"))
		{
			for (const auto& list_ : behaviorTreeGUIDManager_->GetUsedGUIDList())
			{
				ImGui::Text("ID : %d", list_);
			}
			ImGui::TreePop();
		}	
	}
	ImGui::End();
}