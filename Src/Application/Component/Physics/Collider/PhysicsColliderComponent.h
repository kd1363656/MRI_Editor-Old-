#pragma once

class TransformComponent;
class GameObject;

class PhysicsColliderComponent : public ComponentBase
{

public:

	PhysicsColliderComponent ()          = default;
	~PhysicsColliderComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<PhysicsColliderComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	bool Intersects(const KdCollider::SphereInfo& TargetShape , std::list<KdCollider::CollisionResult>* Results);
	bool Intersects(const KdCollider::BoxInfo&    TargetBox   , std::list<KdCollider::CollisionResult>* Results);
	bool Intersects(const KdCollider::RayInfo&    TargetShape , std::list<KdCollider::CollisionResult>* Results);

	void ImGuiPrefabDataInspector() override;

	void           DeserializePrefabData(const nlohmann::json& Json) override;
	nlohmann::json SerializePrefabData  ()                           override;

	std::weak_ptr<KdCollider> GetCollider() { return m_collider; }

	UINT GetCollisionTypeFlags() const { return m_collisionTypeFlags; }

private:

	std::shared_ptr<KdCollider> m_collider = nullptr;

	std::weak_ptr<TransformComponent> m_selfTransformComponent;

	UINT m_collisionTypeFlags = 0u;

};