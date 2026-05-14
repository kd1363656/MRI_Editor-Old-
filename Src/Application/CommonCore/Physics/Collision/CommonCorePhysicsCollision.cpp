#include "CommonCorePhysicsCollision.h"

#include "../../../Component/Physics/Collider/PhysicsColliderComponent.h"

#include "../../../Scene/SceneManager.h"
#include "../../../Scene/BaseScene/BaseScene.h"

#include "../../../Observer/Component/ComponentObserver.h"

#include "../../../Component/Transform/TransformComponent.h"
#include "../../../Component/Physics/DebugWire/PhysicsDebugWireComponent.h"

#include "../../../Utility/BitShift/BitShiftUtility.h"

#include "../../../Tag/TagRegistry.h"

void CommonCorePhysicsCollision::Init()
{
	m_collisionTypeFlags = KdCollider::Type::None;

	m_receiveEventTagID = CommonConstant::INVALID_STATIC_ID;

	m_facingDirectionOffsetMagnification = Math::Vector3::One;

	m_useFacingDirectionAxisFlags = static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::None);

	m_initCollisionEnabled = false;
	m_isCollisionEnabled   = false;
}
void CommonCorePhysicsCollision::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	auto owner_ = Owner;
	if (!owner_) { return; }

	// 当たり判定用のコライダーだけ取得
	if (auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock())
	{
		for (auto gameObject_ : scene_->GetGameObjectList())
		{
			auto collider_ = gameObject_->GetComponent<PhysicsColliderComponent>().lock();
			if (!collider_) { continue; }

			if (owner_->GetGUID() == gameObject_->GetGUID())												 { continue; }
			if (!BitShiftUtility::IsSameFlagStand(m_collisionTypeFlags, collider_->GetCollisionTypeFlags())) { continue; }

			// コライダーが存在していればキャッシュ
			if (!gameObject_->GetComponent<PhysicsColliderComponent>().expired())
			{
				m_collisionTargetList.emplace_back(gameObject_->GetComponent<PhysicsColliderComponent>());
			}
		}
	}
	
	m_componentObserverCache         = owner_->GetComponentObserver				      ();
	m_selfTransformComponentCache    = owner_->GetComponent<TransformComponent>	      ();
	m_physicsDebugWireComponentCache = owner_->GetComponent<PhysicsDebugWireComponent>();

	// "Json"から読み取った初期値を読み込む
	m_isCollisionEnabled = m_initCollisionEnabled;
}

void CommonCorePhysicsCollision::Update()
{
	auto observer_ = m_componentObserverCache.lock();
	if (!observer_) { return; }

	// 登録不可能な"ID"だった場合処理をやめる
	uint32_t eventTagID_ = GetReceiveEventTagID();
	if (eventTagID_ == CommonConstant::INVALID_STATIC_ID) { return; }

	// 当たり判定を実行できるかどうかを判定
	//CommonEnum::BoolFlag canActive_ = observer_->GetEventData(eventTagID_).keep;
	CommonEnum::BoolFlag canActive_ = observer_->GetNotification(eventTagID_ , CommonEnum::EventLane::Keep);

	if (canActive_ == CommonEnum::BoolFlag::True)
	{
		m_isCollisionEnabled = true;
	}
	else if (canActive_ == CommonEnum::BoolFlag::False)
	{
		m_isCollisionEnabled = false;
	}
}

void CommonCorePhysicsCollision::ImGuiPrefabDataInspector()
{
	ImGui::Checkbox("IsCollisionEnabled"   , &m_isCollisionEnabled  );
	ImGui::Checkbox("InitCollisionEnabled" , &m_initCollisionEnabled);
	ImGuiUtility::BitShiftComboSelector("UseFacingDirectionAxisFlags" , m_useFacingDirectionAxisFlags , ImGuiUtility::OFFSET_BY_FACING_FLAGS);

	ImGui::DragFloat3("FacingDirectionOffsetMagnification" , &m_facingDirectionOffsetMagnification.x , 0.1f);

	ImGuiUtility::TagIDRadioButtonSelector("ReceiveTagID"       , m_receiveEventTagID);
	ImGuiUtility::BitShiftComboSelector   ("CollisionTypeFlags" , m_collisionTypeFlags , ImGuiUtility::COLLIDER_CATEGORY_TYPE_LIST);
}

void CommonCorePhysicsCollision::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_receiveEventTagID = JsonUtility::JsonToComponentEventTagID(Json , "ReceiveEventTypeID");

	m_useFacingDirectionAxisFlags = Json.value("UseFacingDirectionAxisFlags" , static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::None));

	if (Json.contains("FacingDirectionOffsetMagnification"))
	{
		m_facingDirectionOffsetMagnification = JsonUtility::JsonToVec3(Json["FacingDirectionOffsetMagnification"]);
	}

	m_initCollisionEnabled = Json.value("InitCollisionEnabled" , false);
	
	m_collisionTypeFlags = Json.value("CollisionTypeFlags" , static_cast<UINT>(KdCollider::Type::None));
}

nlohmann::json CommonCorePhysicsCollision::SerializePrefabData()
{
	auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

	auto json_ = nlohmann::json();

	JsonUtility::ComponentEventTagIDToJson(json_ , m_receiveEventTagID , "ReceiveEventTypeID");
	json_["UseFacingDirectionAxisFlags"] = m_useFacingDirectionAxisFlags;

	json_["FacingDirectionOffsetMagnification"] = JsonUtility::Vec3ToJson(m_facingDirectionOffsetMagnification);

	json_["InitCollisionEnabled"] = m_initCollisionEnabled;
	
	json_["CollisionTypeFlags"] = m_collisionTypeFlags;

	return json_;
}
