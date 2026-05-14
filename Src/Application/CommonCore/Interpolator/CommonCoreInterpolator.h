#pragma once

class CommonCoreMove;

class InterpolatorModifierBase;

class CommonCoreInterpolator : public CommonCoreBase
{

public:

	CommonCoreInterpolator ()          = default;
	~CommonCoreInterpolator() override = default;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

	const CommonStruct::GenericClassData<InterpolatorModifierBase>& GetInterpolatorModifier() const { return m_interpolatorModifier; }

private:

	CommonStruct::GenericClassData<InterpolatorModifierBase> m_interpolatorModifier = {};
};