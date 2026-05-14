#pragma once

class CommonCoreRotation;
class ComponentObserver;

class RotationInputBasedOnTargetInstantComponent : public ComponentBase
{

public:

	RotationInputBasedOnTargetInstantComponent ()          = default;
	~RotationInputBasedOnTargetInstantComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<RotationInputBasedOnTargetInstantComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Update() override;

	void ImGuiSpawnDataInspector() override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData(const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData() override;
	nlohmann::json SerializePrefabData() override;

	std::weak_ptr<CommonCoreRotation> GetComponentCommonCoreRotationCache() const { return m_commonCoreRotation; }

private:

	std::weak_ptr<ComponentObserver> m_componentObserverCache;

	std::shared_ptr<CommonCoreRotation>					      m_commonCoreRotation      = nullptr;
	std::shared_ptr<CommonCoreGUIDBinder<TransformComponent>> m_boundTransformComponent = nullptr;

};