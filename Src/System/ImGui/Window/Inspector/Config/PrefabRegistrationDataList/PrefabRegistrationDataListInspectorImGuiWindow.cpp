#include "PrefabRegistrationDataListInspectorImGuiWindow.h"

#include "../../../../Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"

#include "../Src/Application/Utility/Common/CommonEnum.h"
#include "../Src/Application/Utility/ImGui/ImGuiUtility.h"

#include "../../../../ImGuiManager.h"

void PrefabRegistrationDataListInspectorImGuiWindow::Update()
{
	auto prefabRegistrationDataListImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();
	if (prefabRegistrationDataListImGuiConfig_)
	{
		// 消去予定項目を消去
		std::erase_if(prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList() , [](const auto& Pair) -> bool
		{
			return Pair.second.deleteRequested;
		});
	}

	if (ImGui::Begin("PrefabRegistrationDataListConfigInspector"))
	{
		UpdatePrefabRegistrationDataSelector                ();
		UpdateAddPrefabRegistrationDataButton               ();
		UpdatePrefabRegistrationDataName                    ();
		UpdatePrefabRegistrationDataFilePath				();
		UpdatePrefabRegistrationDataListInspectorImGuiWindow();
	}
	ImGui::End();
}

void PrefabRegistrationDataListInspectorImGuiWindow::UpdatePrefabRegistrationDataSelector()
{
	ImGuiUtility::BitShiftRadioButtonSelector("CategoryType", m_prefabRegistrationData.categoryType, ImGuiUtility::GAME_OBJECT_CATEGORY_TYPE_LIST);
}
void PrefabRegistrationDataListInspectorImGuiWindow::UpdateAddPrefabRegistrationDataButton()
{
	auto& imGuiManager_                          = ImGuiManager::GetInstance();
	auto  prefabRegistrationDataListImGuiConfig_ = imGuiManager_.GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();
	if (!prefabRegistrationDataListImGuiConfig_) { return; }

	ImGui::SameLine();

	if(ImGui::Button("Add PrefabRegistrationData"))
	{
		if (!m_prefabRegistrationData.prefabName.empty())
		{
			prefabRegistrationDataListImGuiConfig_->AddPrefabRegistrationData(m_prefabRegistrationData);
		}
	}
}
void PrefabRegistrationDataListInspectorImGuiWindow::UpdatePrefabRegistrationDataName()
{
	ImGui::InputText("PrefabName", m_inputPrefabName, sizeof(m_inputPrefabName));

	const char* name_ = m_inputPrefabName;
	m_prefabRegistrationData.prefabName = name_;
}
void PrefabRegistrationDataListInspectorImGuiWindow::UpdatePrefabRegistrationDataFilePath()
{
	ImGuiUtility::SelectFilePath("PrefabFilePath", m_prefabRegistrationData.prefabFilePath);
}
void PrefabRegistrationDataListInspectorImGuiWindow::UpdatePrefabRegistrationDataListInspectorImGuiWindow()
{
	auto prefabRegistrationDataListImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();
	if (!prefabRegistrationDataListImGuiConfig_) { return; }

	ImGui::Separator();
	ImGui::Text("PrefabRegistrationDataList");
	ImGui::Separator();

	for (auto& [key_, value_] : prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList())
	{
		if (ImGui::TreeNodeEx(key_.c_str()))
		{		
			// 削除ボタン
			ImGui::SameLine();
			if (ImGui::SmallButton("Delete"))
			{
				value_.deleteRequested = true;
			}
			
			// 各パラメータの描画
			const char* categoryTypeName_ = ImGuiUtility::ConvertEnumListToString(value_.categoryType , ImGuiUtility::GAME_OBJECT_CATEGORY_TYPE_LIST);

			ImGui::Text("PrefabName     : %s" , value_.prefabName.c_str    ());
			ImGui::Text("PrefabFilePath : %s" , value_.prefabFilePath.c_str());
			ImGui::Text("CategoryType   : %s" , categoryTypeName_);

			ImGui::TreePop();
		}

		ImGui::Separator();
	}
}