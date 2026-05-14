#pragma once

class TransformComponent;
class CommonCoreMoveInput;

class InterpolatorModifierBase;

class ComponentObserver;

class MoveInputBasedOnTargetComponent : public ComponentBase
{

public:

	MoveInputBasedOnTargetComponent ()          = default;
	~MoveInputBasedOnTargetComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<MoveInputBasedOnTargetComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Update() override;

	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	std::weak_ptr<CommonCoreMoveInput> GetCommonCoreMoveInputCache() const { return m_commonCoreMoveInput; }

private:

	void FixPos(std::shared_ptr<TransformComponent> SelfTransformComponent);

	std::weak_ptr<ComponentObserver> m_componentObserverCache;

	std::shared_ptr<CommonCoreMoveInput> m_commonCoreMoveInput = nullptr;
	
	CommonStruct::GenericClassData<InterpolatorModifierBase> m_interpolatorModifier = {};

	std::shared_ptr<CommonCoreGUIDBinder<TransformComponent>> m_boundTransformComponent = nullptr;

};