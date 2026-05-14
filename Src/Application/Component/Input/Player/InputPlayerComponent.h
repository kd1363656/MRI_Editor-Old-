#pragma once

class MoveInputBasedOnTargetComponent;
class RotationInputBasedOnTargetSmoothComponent;
class RotationInputBasedOnTargetInstantComponent;
class CommonCoreMoveInput;
class CommonCoreRotation;
class RenderAnimationModelComponent;
class GlobalObserver;

class InputPlayerComponent : public ComponentBase
{

public:

	InputPlayerComponent ()          = default;
	~InputPlayerComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<InputPlayerComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void EarlyUpdate() override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;
	
private:

	void MoveInput    ();
	void RotationInput();
	void AtkInput     ();

	void Move    (GlobalObserver& Observer , std::shared_ptr<CommonCoreMoveInput> CommonCoreMove    );
	void Rotation(GlobalObserver& Observer , std::shared_ptr<CommonCoreRotation>  CommonCoreRotation);

	std::weak_ptr<MoveInputBasedOnTargetComponent>			  m_moveInputBasedOnTargetComponentCache;
	std::weak_ptr<RotationInputBasedOnTargetSmoothComponent>  m_rotationInputBasedOnTargetSmoothComponentCache;
	std::weak_ptr<RotationInputBasedOnTargetInstantComponent> m_rotationInputBasedOnTargetInstantComponentCache;
	std::weak_ptr<RenderAnimationModelComponent>			  m_renderAnimationModelComponentCache;

	bool m_receiveInputAtk = false;
};