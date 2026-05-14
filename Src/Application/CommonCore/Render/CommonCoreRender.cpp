#include "CommonCoreRender.h"

#include "../../Component/Transform/TransformComponent.h"

#include "../AssetFilePath/CommonCoreAssetFilePath.h"

#include "../../GameObject/GameObject.h"

#include "../../Utility/Json/JsonUtility.h"

void CommonCoreRender::Init()
{
	m_color = kWhiteColor;

	if (!m_commonCoreAssetFilePath)
	{
		m_commonCoreAssetFilePath = std::make_shared<CommonCoreAssetFilePath>();
	}
}
void CommonCoreRender::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	if (!Owner) { return; }

	m_selfTransformComponentCache = Owner->GetComponent<TransformComponent>();
}

void CommonCoreRender::ImGuiPrefabDataInspector()
{
	ImGui::ColorEdit4("Color" , &m_color.x);
}

void CommonCoreRender::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_commonCoreAssetFilePath)
	{
		m_commonCoreAssetFilePath->DeserializePrefabData(Json);
	}

	if(Json.contains("Color"))
	{
		m_color = JsonUtility::JsonToColor(Json["Color"]);
	}
}

nlohmann::json CommonCoreRender::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_commonCoreAssetFilePath)
	{
		json_.update(m_commonCoreAssetFilePath->SerializePrefabData());
	}
	json_["Color"] = JsonUtility::ColorToJson(m_color);

	return json_;
}