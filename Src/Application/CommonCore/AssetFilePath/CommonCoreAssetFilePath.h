#pragma once

// アセットのファイルパス管理が責任
class CommonCoreAssetFilePath : public CommonCoreBase
{
public:

	CommonCoreAssetFilePath ()          = default;
	~CommonCoreAssetFilePath() override = default;

	void Init() override;

	void ImGuiPrefabDataInspector() override;
	
	void           DeserializePrefabData(const nlohmann::json& Json) override;
	nlohmann::json SerializePrefabData  ()						     override;
	
	std::string_view GetAssetFilePath() const { return m_assetFilePath; }

	bool GetHasPathChanged() const { return m_hasPathChanged; }

private:

	std::string m_assetFilePath = "";

	bool m_hasPathChanged = false;
};