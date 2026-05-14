#include "CommonCoreRotationInputSmooth.h"

#include "../../CommonCoreRotation.h"

#include "../../../../Component/Transform/TransformComponent.h"

#include "../../../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

void CommonCoreRotationInputSmooth::Init()
{
	if (!m_commonCoreRotation)
	{
		m_commonCoreRotation = std::make_shared<CommonCoreRotation>();
	}

	m_commonCoreRotation->Init();
}
void CommonCoreRotationInputSmooth::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	if (!Owner) { return; }

	if (m_commonCoreRotation)
	{
		m_commonCoreRotation->PostLoadInit(Owner);
	}
}

void CommonCoreRotationInputSmooth::ImGuiPrefabDataInspector()
{
	ImGuiUtility::InterpolatorModifierSelector("InterpolatorSelector", m_interpolatorModifier);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->ImGuiInspector();
	}
}

void CommonCoreRotationInputSmooth::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	const auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance();

	m_interpolatorModifier.name      = Json.value("InterpolatorModifierName" , CommonConstant::STRING_UNKNOWN);
	m_interpolatorModifier.classData = interpolatorModifierFactory_.Create(m_interpolatorModifier.name);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->Init();
		m_interpolatorModifier.classData->DeserializeData(Json);
	}
}

nlohmann::json CommonCoreRotationInputSmooth::SerializePrefabData()
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