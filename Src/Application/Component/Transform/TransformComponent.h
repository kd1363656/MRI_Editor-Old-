#pragma once

class TransformComponent : public ComponentBase									  ,
						   public std::enable_shared_from_this<TransformComponent>
{

public:

	struct Transform
	{
		Math::Quaternion rotation    = Math::Quaternion::Identity;
		Math::Vector3    pos         = Math::Vector3::Zero;
		Math::Vector3    scale       = Math::Vector3::One;
		Math::Matrix	 worldMatrix = Math::Matrix::Identity;
	};

	TransformComponent ()          = default;
	~TransformComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<TransformComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void EarlyUpdate() override;

	void OnCollision    (const std::list<KdCollider::CollisionResult>& Result) override;
	void OnPostCollision()													   override;

	void UpdateStrategy();
	void FixMatrix     ();

	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;
	
	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;
	
	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	Math::Matrix CreateMatrix  () const;
	Math::Matrix GetWorldMatrix();

	Math::Matrix CreateScaleMatrix      () const { return Math::Matrix::CreateScale         (m_transformData.scale);    }
	Math::Matrix CreateRotationMatrix   () const { return Math::Matrix::CreateFromQuaternion(m_transformData.rotation); }
	Math::Matrix CreateTranslationMatrix() const { return Math::Matrix::CreateTranslation   (m_transformData.pos);      }
	
	std::weak_ptr<TransformComponent> GetParentTransformComponent() const { return m_parentTransformComponent; }

	const Math::Matrix& GetMatrix     () const { return m_transformData.worldMatrix; }
	Math::Matrix&		GetWorkMatrix ()		   { return m_transformData.worldMatrix; }
	
	const Math::Quaternion& GetRotation() const { return m_transformData.rotation; }
	
	const Math::Vector3& GetPos		() const { return m_transformData.pos; }
	Math::Vector3        GetWorldPos();

	bool GetCanUpdateMatrix() const { return m_canUpdateMatrix; }
	
	void SetMatrix(const Math::Matrix& Set);

	void SetRotation(const Math::Quaternion& Set);

	void SetPos      (const Math::Vector3& Set);
	void SetPosY     (const float		   Set);
	void SetWorldPos (const Math::Vector3& Set);
	
	void SetLocalPos(const Math::Vector3& Convert);

	void SetIsUpdateMatrixBlocked(const bool Set) { m_isUpdateMatrixBlocked = Set; }
	void SetCanUpdateMatrix		 (const bool Set) { m_canUpdateMatrix	    = Set; }
	
private:

	void CommonImGuiInspector();
	void CheckCanMatrixUpdate();

	std::weak_ptr<TransformComponent>			 m_parentTransformComponent;
	std::list<std::weak_ptr<TransformComponent>> m_childTransformComponent;

	CommonStruct::StrategyData<TransformComponent> m_strategyData = {};

	TransformComponent::Transform m_transformData = {};

	KdCollider::CollisionResult m_sphereCollisionResult = {};
	KdCollider::CollisionResult m_rayCollisionResult    = {};
	
	bool m_isUpdateMatrixBlocked = false;
	bool m_canUpdateMatrix       = true;
};