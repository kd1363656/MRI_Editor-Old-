#pragma once
#include "../../../Utility/ImGui/ImGuiUtility.h"

class CameraComponent;
class CommonCoreRender;

class CommonCoreRenderModel : public CommonCoreBase
{

public:

	CommonCoreRenderModel ()          = default;
	~CommonCoreRenderModel() override = default;

	void Init        ()			   					     override;
	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	std::weak_ptr<CommonCoreGUIDBinder<CameraComponent>> GetBoundCameraComponent() const { return m_boundCameraComponent; }

	std::weak_ptr<CommonCoreRender> GetCommonCoreRenderCache() const { return m_commonCoreRender; }

	uint64_t GetStandardShaderTypeFlags() const { return m_standardShaderTypeFlags; }

	std::unordered_map<CommonEnum::BoneType, std::string>& GetBoneTypeNameList() { return m_boneTypeNameList; }

	const std::string& GetBoneName(const CommonEnum::BoneType BoneType);

	bool GetDisableFrustumCulling() const { return m_disableFrustumCulling; }

private:

	std::shared_ptr<CommonCoreRender>					   m_commonCoreRender     = nullptr;
	std::shared_ptr<CommonCoreGUIDBinder<CameraComponent>> m_boundCameraComponent = nullptr;

	uint64_t m_standardShaderTypeFlags = 0llu;

	std::unordered_map<CommonEnum::BoneType, std::string> m_boneTypeNameList;
	
	bool m_disableFrustumCulling = false;

	std::string			 m_debugAddBoneTypeName							 = "";
	CommonEnum::BoneType m_debugAddBoneType								 = CommonEnum::BoneType::None;
	char				 m_debugInputText[ImGuiUtility::MAX_TEXT_BUFFER] = { 0 };

};