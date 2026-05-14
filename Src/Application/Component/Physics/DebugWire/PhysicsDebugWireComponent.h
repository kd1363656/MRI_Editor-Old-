#pragma once

class PhysicsDebugWireComponent : public ComponentBase
{

public:

	PhysicsDebugWireComponent ()          = default;
	~PhysicsDebugWireComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<PhysicsDebugWireComponent>(); }

	void DrawDebugWire();

	void Init() override;

	std::weak_ptr<KdDebugWireFrame> GetDebugWier() { return m_debugWire; }

	void AddDebugWire(const KdCollider::RayInfo&    RayInfo   );
	void AddDebugWire(const KdCollider::SphereInfo& SphereInfo);

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	std::shared_ptr<KdDebugWireFrame> m_debugWire = nullptr;

	Math::Color m_color = {};

	bool m_canShowDebugWire = false;
};