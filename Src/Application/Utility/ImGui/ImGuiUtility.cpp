#include "ImGuiUtility.h"

#include "../../Resource/ResourceController.h"
#include "../../Resource/Prefab/Prefab.h"
#include "../../Resource/Prefab/PrefabController.h"

#include "../../Factory/Comopnent/ComponentFactory.h"
#include "../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

#include "../../GameObject/GameObject.h"

#include "../Json/JsonUtility.h"

bool ImGuiUtility::SelectFilePath(const char* Label, std::string& FolderPath)
{
	std::string label_ = std::format("{} : {}" , Label , FolderPath);

	if (ImGui::Button((label_.c_str())))
	{
		std::string defPath_ = CommonConstant::ASSET_FOLDER_PATH;

		if (Application::Instance().GetWindow().OpenFileDialog(defPath_))
		{
			// 変更したファイルパスを取得して変数に代入
			FolderPath = defPath_;
			return true;
		}
	}

	return false;
}

void ImGuiUtility::DirectionComboSelector(const char* Label, Math::Vector3& Direction)
{
	ImGui::PushID(&DIRECTION_LIST);

	bool isChanged_ = false;

	if (ImGui::BeginCombo(Label, Label))
	{
		for (const auto& item_ : DIRECTION_LIST)
		{
			bool isSelected_ = (Direction == item_.directionType);

			if (ImGui::Checkbox(item_.label, &isSelected_))
			{
				if (isSelected_)
				{
					Direction += item_.directionType;
					isChanged_ = true;
				}
				else
				{
					Direction -= item_.directionType;
					isChanged_ = true;
				}
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PopID();

	// 方向は正規化して扱うことが多いので"1 ~ 0"に固定しておく
	if (isChanged_)
	{
		Direction.x = std::clamp(Direction.x , Math::Vector3::Left.x     , Math::Vector3::Right.x   );
		Direction.y = std::clamp(Direction.y , Math::Vector3::Down.y     , Math::Vector3::Up.y      );
		Direction.z = std::clamp(Direction.z , Math::Vector3::Forward.z  , Math::Vector3::Backward.z);
	}
}

void ImGuiUtility::DrawSeparate()
{
	ImGui::Spacing  ();
	ImGui::Separator();
	ImGui::Spacing  ();
}

void ImGuiUtility::InterpolatorModifierSelector(const char* Label, CommonStruct::GenericClassData<InterpolatorModifierBase>& Interpolator)
{
	ImGui::PushID(&Interpolator);

	if (ImGui::BeginCombo(Label, Label))
	{
		for (const auto& [key_, value_] : InterpolatorModifierFactory::GetInstance().GetFactoryMethodList())
		{
			bool isSelected_ = (key_ == Interpolator.name);

			if (ImGui::RadioButton(key_.c_str(), isSelected_))
			{
				// もししっかり生成することができたら格納
				auto created_ = InterpolatorModifierFactory::GetInstance().Create(key_);

				if (created_)
				{
					created_->Init();
					Interpolator.name	   = key_;
					Interpolator.classData = created_;
				}
			}

			if (isSelected_)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	ImGui::PopID();
}

const char* ImGuiUtility::BoolToString(bool Flag)
{
	if (!Flag) { return "False"; }
	return "True";
}

const char* ImGuiUtility::BoolToString(CommonEnum::BoolFlag Flag)
{
	if      (Flag == CommonEnum::BoolFlag::True)  { return "True";  }
	else if (Flag == CommonEnum::BoolFlag::False) { return "False"; }
	return "None";
}

void ImGuiUtility::LoadPrefabData(const std::string& PrefabName, std::shared_ptr<GameObject> GameObject)
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	// ゲームオブジェクトの名前からプレハブ情報を取得
	auto prefab_ = prefabController_->GetPrefabData(PrefabName).lock();
	if (!prefab_) { return; }

	// "json"ファイルがなければロードしない
	const auto& json_ = prefab_->GetJsonData();
	if (json_.is_null()) { return; }

	const auto& componentFactory = ComponentFactory::GetInstance();

	if (JsonUtility::CheckIsContainAndArray(json_, "Components"))
	{
		for (const auto& componentData_ : json_["Components"])
		{
			// コンポーネントの名前を取得
			const std::string typeName_ = componentData_.value("TypeName", CommonConstant::STRING_UNKNOWN);

			// 存在したらコンポーネントを生成
			auto component_ = componentFactory.Create(typeName_);

			if (component_)
			{
				component_->Init();
				component_->SetOwner(GameObject);
				// コンポーネント内で"typeid"をつかってクラス名を初期化してもよかったが
				// 処理効率が落ちる、二度手間なのでしなかった
				component_->SetTypeName(typeName_);

				if (componentData_.contains("PrefabData"))
				{
					component_->DeserializePrefabData(componentData_["PrefabData"]);
				}

				// コンポーネントを追加
				GameObject->AddComponent(component_);
				// コンポーネント間の通信を行う
				GameObject->PostLoadInit();
			}
		}
	}
}