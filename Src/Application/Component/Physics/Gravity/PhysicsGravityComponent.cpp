#include "PhysicsGravityComponent.h"
#include "../../Transform/TransformComponent.h"

#include "../../../main.h"

#include "../../../Utility/BitShift/BitShiftUtility.h"

void PhysicsGravityComponent::Init()
{
	m_addGravityForce     = 0.0f;
	m_currentGravityForce = 0.0f;
	m_maxAddGravityForce  = 0.0f;
}
void PhysicsGravityComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	m_selfTransformComponentCache = owner_->GetComponent<TransformComponent>();
}

void PhysicsGravityComponent::Update()
{
	auto selfTransformComponentCache_ = m_selfTransformComponentCache.lock();
	if (!selfTransformComponentCache_) { return; }

	const Math::Vector3 currentPosition_ = selfTransformComponentCache_->GetPos      ();
	const float         deltaTime_       = Application::Instance().GetScaledDeltaTime();

	// 重力を加味した座標を設定
	if (m_canUpdate)
	{
		// 現在の重力が最大重力よりも大きければ加算しない
		if (m_currentGravityForce <= m_maxAddGravityForce)
		{
			m_currentGravityForce += m_addGravityForce * deltaTime_;
		}

		const float newPositionY_ = currentPosition_.y - m_currentGravityForce;
		selfTransformComponentCache_->SetPosY(newPositionY_);
	}
}

void PhysicsGravityComponent::OnCollision(const std::list<KdCollider::CollisionResult>& Result)
{
	// 衝突結果がなければ重力を更新
	if (Result.empty())
	{
		m_canUpdate = true;
		return;
	}

	// もしレイと判定を一回でもしたら重力の更新を止める
	for (const auto& list_ : Result)
	{
		// もし地形との当たり判定でなければ処理を飛ばす
		if (!BitShiftUtility::IsStandFlag(KdCollider::Type::TypeGround, list_.m_collisionType)) { continue; }

		// 地形と当たったかどうかは一回だけ調べるだけでよいから条件が一致したら"break"
		if (list_.m_shapeType == KdCollider::ShapeType::Ray)
		{
			m_currentGravityForce = 0.0f;
			m_canUpdate           = false;
			break;
		}
	}
}

void PhysicsGravityComponent::ImGuiSpawnDataInspector()
{
	ImGui::Text("CurrentForce : %.2f" , m_currentGravityForce);
}
void PhysicsGravityComponent::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat("AddGravityForce"    , &m_addGravityForce    , 0.1f);
	ImGui::DragFloat("MaxAddGravityForce" , &m_maxAddGravityForce , 0.1f);
}

void PhysicsGravityComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_addGravityForce    = Json.value("AddGravityForce"    , 0.0f);
	m_maxAddGravityForce = Json.value("MaxAddGravityForce" , 0.0f);
}
nlohmann::json PhysicsGravityComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["AddGravityForce"   ] = m_addGravityForce;
	json_["MaxAddGravityForce"] = m_maxAddGravityForce;

	return json_;
}