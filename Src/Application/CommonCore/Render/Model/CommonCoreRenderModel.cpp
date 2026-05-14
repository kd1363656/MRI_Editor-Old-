#include "CommonCoreRenderModel.h"
#include "../../../Component/Camera/CameraComponent.h"

#include "../../../GameObject/GameObject.h"

#include "../CommonCoreRender.h"

#include "../../../Utility/ImGui/ImGuiUtility.h"
#include "../../../Utility/Json/JsonUtility.h"
#include "../../../Utility/Component/ComponentUtility.h"

void CommonCoreRenderModel::Init()
{
	if (!m_commonCoreRender)
	{
		m_commonCoreRender = std::make_shared<CommonCoreRender>();
	}

	if (!m_boundCameraComponent)
	{
		m_boundCameraComponent = std::make_shared<CommonCoreGUIDBinder<CameraComponent>>();
	}

	m_commonCoreRender->Init();
	m_boundCameraComponent->Init     ();

	m_standardShaderTypeFlags = static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::None);

	m_disableFrustumCulling = false;
}
void CommonCoreRenderModel::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	if (!Owner) { return; }

	m_commonCoreRender->PostLoadInit(Owner);

	if (m_boundCameraComponent)
	{
		m_boundCameraComponent->PostLoadInit(Owner);
	}
}

void CommonCoreRenderModel::ImGuiSpawnDataInspector()
{
	if (m_boundCameraComponent)
	{
		m_boundCameraComponent->ImGuiSpawnDataInspector();
	}
}
void CommonCoreRenderModel::ImGuiPrefabDataInspector()
{
	if (ImGui::CollapsingHeader("Asset Info"))
	{
		ImGuiUtility::BitShiftComboSelector("ShaderTypeFlags", m_standardShaderTypeFlags, ImGuiUtility::SHADER_TYPE_LIST);

		if (m_commonCoreRender)
		{
			m_commonCoreRender->ImGuiPrefabDataInspector();
		}

		if (m_boundCameraComponent)
		{
			m_boundCameraComponent->ImGuiPrefabDataInspector();
		}

		ImGui::Checkbox("DisableFrustumCulling", &m_disableFrustumCulling);
	}

	if (ImGui::CollapsingHeader("Bone Info"))
	{
		ImGuiUtility::EnumRadioButtonSelector("BoneType", m_debugAddBoneType, ImGuiUtility::BONE_NODE_TYPE_LIST);
		ImGui::InputText("BoneName", m_debugInputText, sizeof(m_debugInputText));

		if (ImGui::Button("Add BoneType"))
		{
			const char* name_ = m_debugInputText;
			m_debugAddBoneTypeName = name_;

			m_boneTypeNameList.try_emplace(m_debugAddBoneType, m_debugAddBoneTypeName);
		}

		for (const auto& [key_, value_] : m_boneTypeNameList)
		{
			ImGuiUtility::DrawSeparate();
			ImGui::Text("BoneTypeName : %s", value_.c_str());
		}
	}
}

void CommonCoreRenderModel::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_boundCameraComponent)
	{
		m_boundCameraComponent->DeserializeSpawnData(Json);
	}	
}
void CommonCoreRenderModel::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_commonCoreRender)
	{
		m_commonCoreRender->DeserializePrefabData(Json);
	}

	if (m_boundCameraComponent)
	{
		m_boundCameraComponent->DeserializePrefabData(Json);
	}
	
	m_standardShaderTypeFlags = Json.value("ShaderTypeFlags" , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::None));

	if (JsonUtility::CheckIsContainAndArray(Json , "BoneTypeNameList"))
	{
		for (const auto& array_ : Json["BoneTypeNameList"])
		{
			const int			 boneType_     = array_.value("BoneType" , static_cast<int>(CommonEnum::BoneType::None));
			CommonEnum::BoneType castBoneType_ = static_cast<CommonEnum::BoneType>(boneType_);

			std::string boneName_ = array_.value("BoneName" , CommonConstant::STRING_UNKNOWN);

			m_boneTypeNameList.try_emplace(castBoneType_ , boneName_);
		}
	}

	m_disableFrustumCulling = Json.value("DisableFrustumCulling" , false);
}

nlohmann::json CommonCoreRenderModel::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	if (m_boundCameraComponent)
	{
		json_.update(m_boundCameraComponent->SerializeSpawnData());
	}
	
	return json_;
}
nlohmann::json CommonCoreRenderModel::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_commonCoreRender)
	{
		json_.update(m_commonCoreRender->SerializePrefabData());
	}

	if (m_boundCameraComponent)
	{
		json_.update(m_boundCameraComponent->SerializePrefabData());
	}

	json_["ShaderTypeFlags"] = m_standardShaderTypeFlags;

	// 各"Enum"に対応したボーン情報を保存
	{
		nlohmann::json arrayJson_ = nlohmann::json::array();
		for(const auto& [key_ , value_] : m_boneTypeNameList)
		{
			auto boneTypeJson_ = nlohmann::json();

			boneTypeJson_["BoneType"] = static_cast<int>(key_);
			boneTypeJson_["BoneName"] = value_;

			arrayJson_.emplace_back(boneTypeJson_);
 		}

		json_["BoneTypeNameList"] = arrayJson_;
	}

	json_["DisableFrustumCulling"] = m_disableFrustumCulling;

	return json_;
}

const std::string& CommonCoreRenderModel::GetBoneName(const CommonEnum::BoneType BoneType)
{
	auto itr_ = m_boneTypeNameList.find(BoneType);

	if (itr_ == m_boneTypeNameList.end()) { return CommonConstant::STRING_UNKNOWN; }

	return itr_->second;
}