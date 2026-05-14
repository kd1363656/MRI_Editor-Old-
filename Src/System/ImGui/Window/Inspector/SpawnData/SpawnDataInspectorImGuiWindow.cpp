#include "SpawnDataInspectorImGuiWindow.h"

#include "../../../ImGuiManager.h"
#include "../../Hierarchy/HierarchyImGuiWindow.h"
#include "../../../Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"

#include "../../../../../Application/GameObject/GameObject.h"

#include "../../../../../Application/Scene/Changer/SceneChanger.h"

#include "../../../../../Application/Resource/ResourceController.h"
#include "../../../../../Application/Resource/Prefab/PrefabController.h"
#include "../../../../../Application/Resource/Prefab/Prefab.h"

#include "../../../../../Application/Factory/Comopnent/ComponentFactory.h"

#include "../../../../../Application/Utility/Json/JsonUtility.h"
#include "../../../../../Application/Utility/ImGui/ImGuiUtility.h"

void SpawnDataInspectorImGuiWindow::Init()
{
	m_selectedComponentName = "Unknown";
}
void SpawnDataInspectorImGuiWindow::Update()
{
	auto& imGuiManager_ = ImGuiManager::GetInstance();

	if (ImGui::Begin("SpawnDataInspector"))
	{
		if (auto hierarchy_ = imGuiManager_.GetImGuiWindow<HierarchyImGuiWindow>().lock())
		{
			if (auto gameObject_ = hierarchy_->GetInspectorWindowTargetGameObject().lock())
			{
				ImGui::Text("GUID : %llu", gameObject_->GetGUID());

				AddComponentSelector(gameObject_);
				AddComponentButton  (gameObject_);
				ComponentInspector  (gameObject_);
			}
		}
	}
	ImGui::End();
}
void SpawnDataInspectorImGuiWindow::ComponentInspector(std::shared_ptr<GameObject> Object) const
{
	if (!Object) { return; }

	auto& componentList_ = Object->GetComponentList();

	// リストの中身がなければ"return"
	if (componentList_.empty()) { return; }

	ImGui::Separator();
	// ユニークコンポーネントの"ImGui"
	ImGui::Text("Unique");
	ComponentInspector(Object , false);
	
	// 余白を作る
	ImGuiUtility::DrawSeparate();

	// マルチコンポーネントの"ImGui"
	ImGui::Text("Multi");
	ComponentInspector(Object, true);

	ImGui::Separator();
}
void SpawnDataInspectorImGuiWindow::AddComponentSelector(std::shared_ptr<GameObject> Object)
{
	if (!Object) { return; }

	ImGui::Text("Component");

	const auto& factory_ = ComponentFactory::GetInstance();

	ImGui::PushID(std::to_address(Object));

	if (ImGui::BeginCombo("##ComponentType", m_selectedComponentName.c_str()))
	{
		for (const auto& [key_, value_] : factory_.GetFactoryMethodList())
		{
			bool isSelected_ = m_selectedComponentName == key_;

			if (ImGui::Selectable(key_.c_str(), &isSelected_))
			{
				m_selectedComponentName = key_;
			}

			// 選択されていたら反映
			if (isSelected_)
			{
				// コンボボックスを開いたとき、キーボード操作（↑↓）で選択を開始できるように、
				// 現在選択されている項目に初期フォーカスを設定する
				ImGui::SetItemDefaultFocus();
			}

		}

		ImGui::EndCombo();
	}

	ImGui::PopID();
}
void SpawnDataInspectorImGuiWindow::AddComponentButton(std::shared_ptr<GameObject> Object) const
{
	if (!Object) { return; }

	ImGui::SameLine();

	if (ImGui::Button("Add Component"))
	{
		Object->AttachComponent(m_selectedComponentName);

		// コンポーネントカノンの通信を行うため"PostLoadInit"
		for (const auto& [key_ , value_] : Object->GetUniqueComponentMap())
		{
			auto component_ = value_.lock();
			if (!component_) { continue; }

			component_->PostLoadInit();
		}
	}
}

void SpawnDataInspectorImGuiWindow::ComponentInspector(std::shared_ptr<GameObject> Object , const bool IsSameType) const
{
	if (!Object) { return; }

	auto& componentList_ = Object->GetComponentList();

	// リストの中身がなければ"return"
	if (componentList_.empty()) { return; }

	// コンポーネントの順序を変えたかどうか
	bool isSwap_ = false;

	for (size_t compI_ = 0llu; compI_ < componentList_.size(); compI_++)
	{
		auto component_ = componentList_[compI_];
		if (!component_) { continue; }

		// マルチコンポーネントかユニークコンポーネント化を外部で指示"return"
		const bool isMulti_ = component_->IsAllowMultiple() == IsSameType;
		if (!isMulti_) { continue; }

		ImGui::PushID(componentList_[compI_].get());

		// コンポーネント内にあるインスペクターを表示
		if (ImGui::TreeNodeEx(component_->GetTypeName().data()))
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("Delete"))
			{
				component_->SetIsDeleteRequested(true);
			}

			// 初めの要素でなければ表示
			if (compI_ != 0llu)
			{
				if (ImGui::ArrowButton("UpBtn", ImGuiDir_Up))
				{
					isSwap_ = true;
					// 要素の中身を入れ替える
					Object->SwapComponentOrder(compI_ , compI_ - 1llu);
				}
			}
			// 次の要素が配列サイズをオーバーしなければ実行
			size_t nextCompIndex_ = compI_ + 1llu;
			if (nextCompIndex_ < componentList_.size())
			{
				// もし"UoButton"があれば実行
				if (compI_ != 0llu)
				{
					ImGui::SameLine();
				}

				if (ImGui::ArrowButton("DownBtn", ImGuiDir_Down))
				{
					isSwap_ = true;
					// 要素の中身を入れ替える
					Object->SwapComponentOrder(compI_ , nextCompIndex_);
				}
			}

			// 要素を変更しているので安全のため"break"
			if (isSwap_)
			{
				ImGui::PopID();
				ImGui::TreePop();
				break;
			}

			ImGui::SameLine();
			if (ImGui::Button("Enable"))
			{
				component_->Enable();
			}
			ImGui::SameLine();
			if (ImGui::Button("Disable"))
			{
				component_->Disable();
			}

			component_->ImGuiSpawnDataInspector();

			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}