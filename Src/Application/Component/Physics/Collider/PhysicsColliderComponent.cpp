#include "PhysicsColliderComponent.h"

#include "../../../GameObject/GameObject.h"

#include "../../Render/Animatoin/Model/RenderAnimationModelComponent.h"
#include "../../Render/Static/Model/RenderStaticModelComponent.h"

#include "../../Transform/TransformComponent.h"

#include "../../../Utility/ImGui/ImGuiUtility.h"

void PhysicsColliderComponent::Init()
{
	if (!m_collider)
	{
		m_collider = std::make_shared<KdCollider>();
	}

	m_collisionTypeFlags = 0u;
}
void PhysicsColliderComponent::PostLoadInit()
{
	// 当たり判定の種類が定義されていなければ登録処理をしない
	if (m_collisionTypeFlags == 0u) { return; }
	if (!m_collider               ) { return; }

	if (auto owner_ = GetOwner())
	{
		// アニメーションモデルだった場合の登録
		if (auto animationModelComponent_ = owner_->GetComponent<RenderAnimationModelComponent>().lock())
		{
			// モデルデータを取得
			auto modelWork_ = animationModelComponent_->GetModelWork().lock();
			if (!modelWork_) { return; }

			m_collider->RegisterCollisionShape
			(
				"RenderAnimationModel",
				modelWork_,
				m_collisionTypeFlags
			);
		}

		if (auto staticModelUniqueComponent_ = owner_->GetComponent<RenderStaticModelComponent>().lock())
		{
			auto modelData_ = staticModelUniqueComponent_->GetModelData().lock();
			if (!modelData_) { return; }

			m_collider->RegisterCollisionShape
			(
				"RenderStaticModel",
				modelData_,
				m_collisionTypeFlags
			);
		}

		// 行列のコンポーネントをセット
		m_selfTransformComponent = owner_->GetComponent<TransformComponent>();
	}
}

bool PhysicsColliderComponent::Intersects(const KdCollider::SphereInfo& TargetShape, std::list<KdCollider::CollisionResult>* Results)
{
	auto selfTransformComponent_ = m_selfTransformComponent.lock();
	if (!selfTransformComponent_) { return false; }
	if (!m_collider             ) { return false; }

	Math::Matrix mat_ = selfTransformComponent_->GetWorldMatrix();

	return m_collider->Intersects(GetOwner() , TargetShape, mat_ , Results);
}
bool PhysicsColliderComponent::Intersects(const KdCollider::BoxInfo& TargetBox, std::list<KdCollider::CollisionResult>* Results)
{
	auto selfTransformComponent_ = m_selfTransformComponent.lock();
	if (!selfTransformComponent_) { return false; }
	if (!m_collider			    ) { return false; }

	Math::Matrix mat_ = selfTransformComponent_->GetWorldMatrix();

	return m_collider->Intersects(GetOwner() , TargetBox , mat_ , Results);
}
bool PhysicsColliderComponent::Intersects(const KdCollider::RayInfo& TargetShape, std::list<KdCollider::CollisionResult>* Results)
{
	auto selfTransformComponent_ = m_selfTransformComponent.lock();
	if (!selfTransformComponent_) { return false; }
	if (!m_collider			    ) { return false; }

	Math::Matrix mat_ = selfTransformComponent_->GetWorldMatrix();

	return m_collider->Intersects(GetOwner() , TargetShape , mat_ , Results);
}

void PhysicsColliderComponent::ImGuiPrefabDataInspector()
{
	const UINT oldCollisionType_ = m_collisionTypeFlags;

	ImGuiUtility::BitShiftComboSelector("CollisionType", m_collisionTypeFlags, ImGuiUtility::COLLIDER_CATEGORY_TYPE_LIST);

	// ビットフラグが変更されたら当たり判定を登録
	if (oldCollisionType_ == m_collisionTypeFlags) { return; }

	if (auto owner_ = GetOwner())
	{
		// アニメーションモデルだった場合の登録
		if (auto animationModelComponent_ = owner_->GetComponent<RenderAnimationModelComponent>().lock())
		{
			// モデルデータを取得
			auto modelWork_ = animationModelComponent_->GetModelWork().lock();
			if (!modelWork_) { return; }

			m_collider->RegisterCollisionShape
			(
				"AnimationModel",
				modelWork_,
				m_collisionTypeFlags
			);
		}

		if (auto staticModelUniqueComponent_ = owner_->GetComponent<RenderStaticModelComponent>().lock())
		{
			auto modelData_ = staticModelUniqueComponent_->GetModelData().lock();
			if (!modelData_) { return; }

			m_collider->RegisterCollisionShape
			(
				"StaticModel",
				modelData_,
				m_collisionTypeFlags
			);
		}
	}
}

void PhysicsColliderComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_collisionTypeFlags = Json.value("CollisionType", 0u);
}
nlohmann::json PhysicsColliderComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["CollisionType"] = m_collisionTypeFlags;

	return json_;
}