#include "CommonCoreInterpolator.h"

#include "../../Component/Transform/TransformComponent.h"

#include "../../GameObject/GameObject.h"

#include "../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

#include "../../main.h"

void CommonCoreInterpolator::ImGuiPrefabDataInspector()
{
	ImGuiUtility::InterpolatorModifierSelector("InterpolatorSelector", m_interpolatorModifier);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->ImGuiInspector();
	}
}

void CommonCoreInterpolator::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	const auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance();

	m_interpolatorModifier.name = Json.value("InterpolatorModifierName", CommonConstant::STRING_UNKNOWN);
	m_interpolatorModifier.classData = interpolatorModifierFactory_.Create(m_interpolatorModifier.name);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->Init();
		m_interpolatorModifier.classData->DeserializeData(Json);
	}
}
nlohmann::json CommonCoreInterpolator::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_interpolatorModifier.classData)
	{
		auto interpolatorJson_ = m_interpolatorModifier.classData->SerializeData();

		if (!interpolatorJson_.is_null())
		{
			json_.update(interpolatorJson_);
		}

		json_["InterpolatorModifierName"] = m_interpolatorModifier.name;
	}

	return json_;
}