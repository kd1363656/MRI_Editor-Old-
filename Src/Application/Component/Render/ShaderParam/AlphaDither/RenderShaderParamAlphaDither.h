#pragma once
#include "../RenderShaderParamBase.h"

class RenderShaderParamAlphaDither : public RenderShaderParamBase
{

public:

	RenderShaderParamAlphaDither ()          = default;
	~RenderShaderParamAlphaDither() override = default;

	void Init() override;

	void EnableParamEffect () override;
	void DisableParamEffect() override;
	
	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;


private:

	float m_enableDistance = 0.0f;

	bool m_isEnable = true;
};