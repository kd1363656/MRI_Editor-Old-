#include "RenderShaderParamAlphaDither.h"

void RenderShaderParamAlphaDither::Init()
{
	m_enableDistance = 0.0f;
	m_isEnable       = true;
}

void RenderShaderParamAlphaDither::EnableParamEffect()
{
	KdShaderManager::Instance().m_StandardShader.EnableAlphaDither(m_isEnable , m_enableDistance);
}
void RenderShaderParamAlphaDither::DisableParamEffect()
{
	KdShaderManager::Instance().m_StandardShader.DisableAlphaDither();
}

void RenderShaderParamAlphaDither::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat("EnableDistance" , &m_enableDistance);
	ImGui::Checkbox ("IsEnable"       , &m_isEnable      );
}

void RenderShaderParamAlphaDither::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_enableDistance = Json.value("EnableDistance" , 0.0f );
	m_isEnable       = Json.value("IsEnable"       , false);
}
nlohmann::json RenderShaderParamAlphaDither::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["EnableDistance"] = m_enableDistance;
	json_["IsEnable"      ] = m_isEnable;

	return json_;
}