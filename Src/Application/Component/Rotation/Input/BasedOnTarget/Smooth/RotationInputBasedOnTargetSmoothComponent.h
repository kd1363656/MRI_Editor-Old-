#pragma once

class CommonCoreRotationInputSmooth;
class ComponentObserver;

class RotationInputBasedOnTargetSmoothComponent : public ComponentBase
{

public:

	RotationInputBasedOnTargetSmoothComponent ()          = default;
	~RotationInputBasedOnTargetSmoothComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<RotationInputBasedOnTargetSmoothComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Update() override;

	void ImGuiSpawnDataInspector() override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData(const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData() override;
	nlohmann::json SerializePrefabData() override;

	std::weak_ptr<CommonCoreRotationInputSmooth> GetCommonCoreRotationInputSmoothCache() const { return m_commonCoreRotationInputSmooth; }

private:

	std::weak_ptr<ComponentObserver> m_componentObserverCache;

	std::shared_ptr<CommonCoreRotationInputSmooth>			  m_commonCoreRotationInputSmooth = nullptr;
	std::shared_ptr<CommonCoreGUIDBinder<TransformComponent>> m_boundTransformComponent	      = nullptr;

};