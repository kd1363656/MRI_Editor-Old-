#include "PhysicsCollisionRayComponent.h"
#include "../../../Transform/TransformComponent.h"
#include "../../DebugWire/PhysicsDebugWireComponent.h"
#include "../../Collider/PhysicsColliderComponent.h"
#include "../../Gravity/PhysicsGravityComponent.h"

#include "../../../../CommonCore/Physics/Collision/CommonCorePhysicsCollision.h"

#include "../../../../GameObject/GameObject.h"

#include "../../../../Observer/Component/ComponentObserver.h"

#include "../../../../Utility/Json/JsonUtility.h"
#include "../../../../Utility/ImGui/ImGuiUtility.h"
#include "../../../../Utility/BitShift/BitShiftUtility.h"

void PhysicsCollisionRayComponent::Init()
{
	if (!m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision = std::make_shared<CommonCorePhysicsCollision>();
	}

	m_commonCorePhysicsCollision->Init();

	m_rayCollisionData = {};

	// デバックのレンダーがエラーを出さないよう下方向で初期化
	m_rayCollisionData.direction = Math::Vector3::Down;
}
void PhysicsCollisionRayComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (auto parent_ = owner_->GetParent().lock())
	{
		m_parentTransformComponent = parent_->GetComponent<TransformComponent>();
	}
	
	if (m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision->PostLoadInit(owner_);
	}

	m_physicsGravityComponent = owner_->GetComponent<PhysicsGravityComponent>();
}

void PhysicsCollisionRayComponent::Update()
{
	auto commonCore_ = m_commonCorePhysicsCollision;
	if (!commonCore_) { return; }

	commonCore_->Update();
}

void PhysicsCollisionRayComponent::LateUpdate()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto commonCorePhysicsCollision_ = m_commonCorePhysicsCollision;
	if (!commonCorePhysicsCollision_) { return; }

	auto selfTransformComponentCache_ = commonCorePhysicsCollision_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }
	
	// 当たり判定が"Off"なら処理を実行しない
	if (!commonCorePhysicsCollision_->GetIsCollisionEnabled()) { return; }

	// レイの設定
	KdCollider::RayInfo rayInfo_ = {};

	// 親の方向に対してレイキャストをするのかどうかで代入する値を変える
	rayInfo_.m_pos   += selfTransformComponentCache_->GetWorldPos() + m_rayCollisionData.offset;
	rayInfo_.m_range  = m_rayCollisionData.range; 
	rayInfo_.m_dir    = m_rayCollisionData.direction;
	rayInfo_.m_type   = commonCorePhysicsCollision_->GetCollisionTypeFlags();

	// デバックワイヤーの追加
	if (auto collisionDebugWire_ = commonCorePhysicsCollision_->GetPhysicsDebugWireComponentCache().lock())
	{
		collisionDebugWire_->AddDebugWire(rayInfo_);
	}

	std::list<KdCollider::CollisionResult> resultList_ = {};
	for (const auto& list_ : commonCorePhysicsCollision_->GetWorkCollisionTargetList())
	{
		auto collider_ = list_.lock();
		if (!collider_) { continue; }

		collider_->Intersects(rayInfo_ , &resultList_);
	}

	// 自身のコンポーネントに接触したコンポーネントとの当たり判定を伝える
	for (auto& component_ : owner_->GetComponentList())
	{
		component_->OnCollision(resultList_);
	}
}

void PhysicsCollisionRayComponent::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat3("Offset" , &m_rayCollisionData.offset.x , 0.1f);
	ImGui::DragFloat ("Range"  , &m_rayCollisionData.range    , 0.1f);

	ImGuiUtility::DirectionComboSelector ("RayDirection"       , m_rayCollisionData.direction);

	if (m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision->ImGuiPrefabDataInspector();
	}
}

void PhysicsCollisionRayComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (Json.contains("Offset"   ))
	{
		m_rayCollisionData.offset = JsonUtility::JsonToVec3(Json["Offset"   ]);
	}

	if (Json.contains("Direction")) 
	{
		m_rayCollisionData.direction = JsonUtility::JsonToVec3(Json["Direction"]);
	}

	m_rayCollisionData.range = Json.value("Range" , 0.0f);
	
	if (m_commonCorePhysicsCollision)
	{
		m_commonCorePhysicsCollision->DeserializePrefabData(Json);
	}
}
nlohmann::json PhysicsCollisionRayComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["Offset"            ] = JsonUtility::Vec3ToJson(m_rayCollisionData.offset   );
	json_["Direction"         ] = JsonUtility::Vec3ToJson(m_rayCollisionData.direction);
	json_["Range"             ] = m_rayCollisionData.range;

	if (m_commonCorePhysicsCollision)
	{
		json_.update(m_commonCorePhysicsCollision->SerializePrefabData());
	}

	return json_;
}