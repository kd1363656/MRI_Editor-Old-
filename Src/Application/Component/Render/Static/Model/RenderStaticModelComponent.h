#pragma once
#include "../../Model/RenderModelComponentBase.h"
#include "../../../../Utility/ImGui/ImGuiUtility.h"

class CommonCoreRenderModel;

class RenderStaticModelComponent : public RenderModelComponentBase
{

public:

	RenderStaticModelComponent ()          = default;
	~RenderStaticModelComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<RenderStaticModelComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Draw(const CommonEnum::StandardShaderTypeFlags Flag) override;

	void ImGuiSpawnDataInspector() override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	bool IsInFrustum() const override;

	std::weak_ptr<KdModelData> GetModelData() const { return m_modelData; }

	Math::Matrix GetBoneWorldMatrix(const CommonEnum::BoneType BoneType) override;

private:

	void LoadModel();

	std::shared_ptr<KdModelData> m_modelData = nullptr;

};