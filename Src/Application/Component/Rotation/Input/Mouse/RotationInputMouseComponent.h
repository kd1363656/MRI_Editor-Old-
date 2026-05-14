#pragma once

class CommonCoreRotation;

class RotationInputMouseComponent : public ComponentBase
{

public:

	RotationInputMouseComponent ()          = default;
	~RotationInputMouseComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<RotationInputMouseComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Update() override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	void MouseLock      ();
	void UpdateRotation ();

	std::shared_ptr<CommonCoreRotation> m_commonCoreRotation = nullptr;

	Math::Vector3 m_targetRotation = Math::Vector3::Zero;

	float m_minRotatableDegreeX = 0.0f;
	float m_maxRotatableDegreeX = 0.0f;

	float m_maxRotationSpeed = 0.0f;

	bool m_disableMouseLock = false;
};