#include "SpringArmComponent.h"

#include "../Transform/TransformComponent.h"
#include "../Physics/Collider/PhysicsColliderComponent.h"
#include "../Physics/DebugWire/PhysicsDebugWireComponent.h"

#include "../../Scene/SceneManager.h"
#include "../../Scene/BaseScene/BaseScene.h"

#include "../../Utility/Json/JsonUtility.h"
#include "../../Utility/BitShift/BitShiftUtility.h"
#include "../../Utility/ImGui/ImGuiUtility.h"

void SpringArmComponent::Init()
{
	m_standardOffsetPos = Math::Vector3::Zero;
	
	m_springSpeed = CommonConstant::FLOAT_ONE;

	m_bumpMagnification = CommonConstant::FLOAT_ONE;

	m_collisionType = 0u;
}
void SpringArmComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	// 普段のカメラが離れている位置が現在の位置になる
	if (auto transformComponent_ = owner_->GetComponent<TransformComponent>().lock())
	{
		transformComponent_->SetPos(m_standardOffsetPos);
		m_selfTransformComponent = transformComponent_;
	}

	if (auto parent_ = owner_->GetParent().lock())
	{
		m_parentTransformComponent = parent_->GetComponent<TransformComponent>();
	}

	// 当たり判定用のコライダーだけ取得
	if (auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock())
	{
		for (auto gameObject_ : scene_->GetGameObjectList())
		{
			auto collider_ = gameObject_->GetComponent<PhysicsColliderComponent>().lock();
			if (!collider_) { continue; }

			if (owner_->GetGUID() == gameObject_->GetGUID())											 { continue; }
			if (!BitShiftUtility::IsSameFlagStand(m_collisionType , collider_->GetCollisionTypeFlags())) { continue; }

			// コライダーが存在していればキャッシュ
			if (!gameObject_->GetComponent<PhysicsColliderComponent>().expired())
			{
				m_hitObjectList.emplace_back(gameObject_->GetComponent<PhysicsColliderComponent>());
			}
		}
	}

	m_physicsDebugWireComponent = owner_->GetComponent<PhysicsDebugWireComponent>();
}

void SpringArmComponent::OnPostCollisionResolved()
{
	auto selfTransformComponent_ = m_selfTransformComponent.lock();
	if (!selfTransformComponent_) { return; }

	auto parentTransformComponent_ = m_parentTransformComponent.lock();
	if (!parentTransformComponent_) { return; }

	// 地形と当たったかどうかを判定
	bool hit_ = false;

	{
		// レイの設定
		KdCollider::RayInfo rayInfo_ = {};

		rayInfo_.m_pos = selfTransformComponent_->GetWorldPos();

		// 自身とターゲットの座標を取得
		Math::Vector3 startPos_ = selfTransformComponent_->GetWorldPos();
		Math::Vector3 targetPos_ = parentTransformComponent_->GetWorldPos() + Math::Vector3::Up;

		rayInfo_.m_dir = targetPos_ - startPos_;
		rayInfo_.m_range = rayInfo_.m_dir.Length();
		rayInfo_.m_dir.Normalize();

		rayInfo_.m_type = m_collisionType;

		std::list<KdCollider::CollisionResult> resultRayList;
		for (const auto& list_ : m_hitObjectList)
		{
			auto collider_ = list_.lock();
			if (!collider_) { continue; }

			// 同じビットシフトが立っていなければ"return"
			if (!BitShiftUtility::IsSameFlagStand(collider_->GetCollisionTypeFlags(), rayInfo_.m_type)) { continue; }
			collider_->Intersects(rayInfo_, &resultRayList);
		}

		float         maxOverLap_ = 0.0f;
		Math::Vector3 hitPos_	  = {};
	
		for (const auto& ret_ : resultRayList)
		{
			// レイを遮断しオーバーした長さが
			// 一番長いものを探す
			if (maxOverLap_ < ret_.m_overlapDistance)
			{
				maxOverLap_ = ret_.m_overlapDistance;
				hitPos_ = ret_.m_hitPos;
				hit_ = true;
			}
		}

		// ヒットした個所に自身を移動する
		if (hit_)
		{
			Math::Vector3 resultHitPos_ = hitPos_;
			resultHitPos_ += (rayInfo_.m_dir * m_bumpMagnification);

			// 行列を互生して拡大、回転、親の行列を入れてから"Translation"に直接埋め込む
			// そうしないと安定した挙動にならないから
			selfTransformComponent_->SetWorldPos(resultHitPos_);
		}
	}
}

void SpringArmComponent::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat3("StandardOffsetPos" , &m_standardOffsetPos.x , 0.1f);
	ImGui::DragFloat ("SpringSpeed"       , &m_springSpeed         , 0.1f);
	ImGui::DragFloat ("BumpMagnification" , &m_bumpMagnification   , 0.1f);

	ImGuiUtility::BitShiftComboSelector("CollisionType" , m_collisionType , ImGuiUtility::COLLIDER_CATEGORY_TYPE_LIST);
}

void SpringArmComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (Json.contains("StandardOffsetPos"))
	{
		m_standardOffsetPos = JsonUtility::JsonToVec3(Json["StandardOffsetPos"]);
	}

	m_springSpeed	    = Json.value("SpringSpeed"		 , 0.0f						);
	m_bumpMagnification = Json.value("BumpMagnification" , CommonConstant::FLOAT_ONE);

	m_collisionType = Json.value("CollisionType" , static_cast<UINT>(KdCollider::Type::None));
}

nlohmann::json SpringArmComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["StandardOffsetPos"] = JsonUtility::Vec3ToJson(m_standardOffsetPos);

	json_["SpringSpeed"      ] = m_springSpeed;
	json_["BumpMagnification"] = m_bumpMagnification;

	json_["CollisionType"] = m_collisionType;

	return json_;
}