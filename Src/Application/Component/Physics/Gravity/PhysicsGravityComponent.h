#pragma once

class TransformComponent;

class PhysicsGravityComponent : public ComponentBase
{

public:

	PhysicsGravityComponent ()          = default;
	~PhysicsGravityComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<PhysicsGravityComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Update() override;

	void OnCollision(const std::list<KdCollider::CollisionResult>& Result) override;

	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;

	void           DeserializePrefabData(const nlohmann::json& Json) override;
	nlohmann::json SerializePrefabData  ()                           override;

	void SetCurrentGravityForce(float Set) { m_currentGravityForce = Set; }

	float GetCurrentGravityForce() const { return m_currentGravityForce; }

private:

	std::weak_ptr<TransformComponent> m_selfTransformComponentCache;

	float m_currentGravityForce = 0.0f;
	float m_addGravityForce     = 0.0f;
	float m_maxAddGravityForce  = 0.0f;

	bool m_canUpdate = false;
};