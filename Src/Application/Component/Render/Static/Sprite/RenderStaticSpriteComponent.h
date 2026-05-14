#pragma once
#include "../../Model/RenderModelComponentBase.h"
#include "../../../../Utility/ImGui/ImGuiUtility.h"

class RenderStaticSpriteComponent : public RenderModelComponentBase
{

public:

	RenderStaticSpriteComponent ()          = default;
	~RenderStaticSpriteComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<RenderStaticSpriteComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Draw(const CommonEnum::StandardShaderTypeFlags Flag) override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:


	std::shared_ptr<KdTexture> m_tex = nullptr;

};