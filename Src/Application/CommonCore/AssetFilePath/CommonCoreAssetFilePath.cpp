#include "CommonCoreAssetFilePath.h"

#include "../../Utility/ImGui/ImGuiUtility.h"

void CommonCoreAssetFilePath::Init()
{
	m_assetFilePath.clear();
	m_hasPathChanged = false;
}

void CommonCoreAssetFilePath::ImGuiPrefabDataInspector()
{
	// ファイルパスの変更を受け取る
	m_hasPathChanged = ImGuiUtility::SelectFilePath("AssetFilePath", m_assetFilePath);
}

void CommonCoreAssetFilePath::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_assetFilePath = Json.value("AssetFilePath", "");
	
}
nlohmann::json CommonCoreAssetFilePath::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["AssetFilePath"] = m_assetFilePath;

	return json_;
}