#pragma once
#include "../../Render/ShaderParam/RenderShaderParamBase.h"

class CommonCoreRenderModel;

class RenderModelComponentBase : public ComponentBase
{

public:

	RenderModelComponentBase ()          = default;
	~RenderModelComponentBase() override = default;

	void Init        () override;
	void PostLoadInit() override;

	void ImGuiSpawnDataInspector  () override;
	void ImGuiPrefabDataInspector () override;
	
	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	virtual void Draw		(const CommonEnum::StandardShaderTypeFlags Flag)       = 0;
	virtual bool IsInFrustum()												 const = 0;

	std::weak_ptr<CommonCoreRenderModel> GetCommonCoreRenderModelCache() const { return m_commonCoreRenderModel; }

	const std::vector<CommonStruct::GenericClassData<RenderShaderParamBase>>& GetRenderShaderParamList() const { return m_renderShaderParamList; }

	virtual Math::Matrix GetBoneWorldMatrix(const CommonEnum::BoneType BoneType) = 0;

private:

	void AddRenderShaderParamList(CommonStruct::GenericClassData<RenderShaderParamBase> WantAdd);

	std::vector<CommonStruct::GenericClassData<RenderShaderParamBase>> m_renderShaderParamList;

	std::shared_ptr<CommonCoreRenderModel> m_commonCoreRenderModel = nullptr;

};