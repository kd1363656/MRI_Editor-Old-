#include "InterpolatorModifierBase.h"

void InterpolatorModifierBase::Init()
{
	m_progress = 0.0f;
	
	m_maxMeasurementTime = CommonConstant::FLOAT_ONE;
	m_elapsedTime		 = 0.0f;

	m_clampMaxValue = CommonConstant::FLOAT_ONE;
	m_currentValue  = 0.0f;
}

void InterpolatorModifierBase::ImGuiInspector()
{
	ImGui::Text("Progress     : %.2f" , m_progress    );
	ImGui::Text("ElapsedTime  : %.2f" , m_elapsedTime );
	ImGui::Text("CurrentValue : %.2f" , m_currentValue);

	ImGui::DragFloat("MaxMeasurementTime" , &m_maxMeasurementTime , 0.1f);
	ImGui::DragFloat("ClampMaxValue"      , &m_clampMaxValue	  , 0.1f);

	// 各種パラメータの最大値が"0"を下回らないようにする
	m_maxMeasurementTime = std::max(m_maxMeasurementTime , CommonConstant::FLOAT_ONE);
	m_clampMaxValue      = std::max(m_clampMaxValue      , CommonConstant::FLOAT_ONE);
}

void InterpolatorModifierBase::DeserializeData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_maxMeasurementTime = Json.value("MaxMeasurementTime" , 0.0f					  );
	m_clampMaxValue      = Json.value("ClampMaxValue"      , CommonConstant::FLOAT_ONE);
}

nlohmann::json InterpolatorModifierBase::SerializeData()
{
	auto json_ = nlohmann::json();

	json_["MaxMeasurementTime"] = m_maxMeasurementTime;
	json_["ClampMaxValue"     ] = m_clampMaxValue;

	return json_;
}

void InterpolatorModifierBase::ResetInterpolate()
{
	m_progress = 0.0f;

	m_elapsedTime = 0.0f;

	m_currentValue = 0.0f;
}