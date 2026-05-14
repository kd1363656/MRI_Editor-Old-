#pragma once

class CommonCoreRotation;
class InterpolatorModifierBase;

class CommonCoreRotationInputSmooth : public CommonCoreBase
{

public:

	CommonCoreRotationInputSmooth ()          = default;
	~CommonCoreRotationInputSmooth() override = default;

	void Init        ()									 override;
	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

	std::weak_ptr<CommonCoreRotation> GetCommonCoreRotationCache() const { return m_commonCoreRotation; }

	const CommonStruct::GenericClassData<InterpolatorModifierBase>& GetInterpolatorModifierCache() const { return m_interpolatorModifier; }

private:

	std::shared_ptr<CommonCoreRotation> m_commonCoreRotation = nullptr;

	CommonStruct::GenericClassData<InterpolatorModifierBase> m_interpolatorModifier = {};

};