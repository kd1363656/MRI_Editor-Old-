#pragma once

class TransformComponent;
class CommonCoreAssetFilePath;

class CommonCoreRender : public CommonCoreBase
{

public:

	CommonCoreRender ()          = default;
	~CommonCoreRender() override = default;

	void Init        ()									 override;
	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;
	
	std::weak_ptr<TransformComponent> GetSelfTransformComponentCache() const { return m_selfTransformComponentCache; }

	std::weak_ptr<CommonCoreAssetFilePath> GetCommonCoreAssetFilePathCache() const { return m_commonCoreAssetFilePath; }

	const Math::Color& GetColor() const { return m_color; }

private:

	std::weak_ptr<TransformComponent> m_selfTransformComponentCache;

	std::shared_ptr<CommonCoreAssetFilePath> m_commonCoreAssetFilePath = nullptr;

	Math::Color m_color = kWhiteColor;
};