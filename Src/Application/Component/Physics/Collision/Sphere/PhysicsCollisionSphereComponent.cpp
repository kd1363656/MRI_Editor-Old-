#include "PhysicsCollisionSphereComponent.h"
#include "../../../Transform/TransformComponent.h"
#include "../../DebugWire/PhysicsDebugWireComponent.h"
#include "../../Collider/PhysicsColliderComponent.h"

#include "../../../../CommonCore/Physics/Collision/CommonCorePhysicsCollision.h"

#include "../../../../GameObject/GameObject.h"

#include "../../../../Observer/Component/ComponentObserver.h"

#include "../../../../Utility/Json/JsonUtility.h"
#include "../../../../Utility/ImGui/ImGuiUtility.h"
#include "../../../../Utility/BitShift/BitShiftUtility.h"

void PhysicsCollisionSphereComponent::Init()
{
	if (!m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision = std::make_shared<CommonCorePhysicsCollision>();
	}

	m_commonCorePhysicsCollision->Init();

	m_sphereCollisionData = {};
}
void PhysicsCollisionSphereComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision->PostLoadInit(owner_);
	}
}

void PhysicsCollisionSphereComponent::Update()
{
	auto commonCore_ = m_commonCorePhysicsCollision;
	if (!commonCore_) { return; }

	commonCore_->Update();
}

void PhysicsCollisionSphereComponent::LateUpdate()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto commonCorePhysicsCollision_ = m_commonCorePhysicsCollision;
	if (!commonCorePhysicsCollision_) { return; }

	auto selfTransformComponentCache_ = commonCorePhysicsCollision_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	// 当たり判定が"Off"なら処理を実行しない
	if (!commonCorePhysicsCollision_->GetIsCollisionEnabled()) { return; }

	// 球判定の設定
	KdCollider::SphereInfo sphereInfo_ = {};
	sphereInfo_.m_sphere.Center = selfTransformComponentCache_->GetWorldPos();
	sphereInfo_.m_sphere.Radius = m_sphereCollisionData.radius;
	sphereInfo_.m_type          = commonCorePhysicsCollision_->GetCollisionTypeFlags();
	
	uint32_t offsetDirection_ = commonCorePhysicsCollision_->GetUseFacingDirectionAxisFlags();
	
	Math::Vector3 offset_ = Math::Vector3::Zero;
	offset_ = m_sphereCollisionData.offset;

	// 自身の向いている方向を基準としたオフセット適応(フラグが立っていたら実行)
	if (offsetDirection_ != static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::None))
	{
		// 注意、モデルの向きに依存
		Math::Vector3 facingDirection_ = Math::Vector3::TransformNormal(Math::Vector3::Forward , selfTransformComponentCache_->GetWorldMatrix());
		facingDirection_.Normalize();
		
		Math::Vector3 magnification_ = commonCorePhysicsCollision_->GetFacingDirectionOffsetMagnification();

		// 方向ベクトルとオフセット倍率を掛けたものをスフィア一のオフセットに適用
		if (BitShiftUtility::IsStandFlag(CommonEnum::UseFacingDirectionAxisFlags::X , offsetDirection_))
		{
			offset_.x = m_sphereCollisionData.offset.x + (facingDirection_.x * magnification_.x);
		}
		if (BitShiftUtility::IsStandFlag(CommonEnum::UseFacingDirectionAxisFlags::Y, offsetDirection_))
		{
			offset_.y = m_sphereCollisionData.offset.y + (facingDirection_.y * magnification_.y);
		}
		if (BitShiftUtility::IsStandFlag(CommonEnum::UseFacingDirectionAxisFlags::Z, offsetDirection_))
		{
			offset_.z = m_sphereCollisionData.offset.z + (facingDirection_.z * magnification_.z);
		}
	}
	
	sphereInfo_.m_sphere.Center.x += offset_.x;
	sphereInfo_.m_sphere.Center.y += offset_.y;
	sphereInfo_.m_sphere.Center.z += offset_.z;

	// デバックワイヤーの追加
	if (auto collisionDebugWire_ = commonCorePhysicsCollision_->GetPhysicsDebugWireComponentCache().lock())
	{
		collisionDebugWire_->AddDebugWire(sphereInfo_);
	}

	std::list<KdCollider::CollisionResult> resultList_ = {};
	for (const auto& list_ : commonCorePhysicsCollision_->GetWorkCollisionTargetList())
	{
		auto collider_ = list_.lock();
		if (!collider_) { continue; }

		collider_->Intersects(sphereInfo_, &resultList_);
	}

	// 自身のコンポーネントに接触したコンポーネントとの当たり判定を伝える
	for (auto& component_ : owner_->GetComponentList())
	{
		component_->OnCollision(resultList_);
	}
}

void PhysicsCollisionSphereComponent::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat3("Offset" , &m_sphereCollisionData.offset.x , 0.1f);
	ImGui::DragFloat ("Radius" , &m_sphereCollisionData.radius   , 0.1f);

	if (m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision->ImGuiPrefabDataInspector();
	}
}

void PhysicsCollisionSphereComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (Json.contains("Offset"))
	{
		m_sphereCollisionData.offset = JsonUtility::JsonToVec3(Json["Offset"]);
	}

	m_sphereCollisionData.radius = Json.value("Radius" , 0.0f);

	if (m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision->DeserializePrefabData(Json);
	}
}
nlohmann::json PhysicsCollisionSphereComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["Offset"] = JsonUtility::Vec3ToJson(m_sphereCollisionData.offset);
	json_["Radius"] = m_sphereCollisionData.radius;

	if (m_commonCorePhysicsCollision)
	{
		json_.update(m_commonCorePhysicsCollision->SerializePrefabData());
	}

	return json_;
}