#include "CombatComponent.h"

#include "../../Utility/BitShift/BitShiftUtility.h"

#include "../../Observer/Component/ComponentObserver.h"

#include "../../Tag/ComponentEvent/ComponentEventTag.h"
#include "../../Tag/TagRegistry.h"

void CombatComponent::Init()
{
	m_atkPow = 0.0f;
	m_defPow = 0.0f;
	m_nowHp  = 0.0f;
	m_maxHp  = 0.0f;

	m_hasAttackHit = false;
}

void CombatComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (auto componentObserver_ = owner_->GetComponentObserver().lock())
	{
		m_componentObserverCache = componentObserver_;
	}
}

void CombatComponent::TakeDamage(const float AtkPow)
{
	// ドラゴンクエストのダメージ計算方式
	const float damage_ = (AtkPow / 2.0f) - (m_defPow / 4.0f);
	const float hp_     = m_nowHp - damage_;

	const float resultHp_ = std::max(hp_ , 0.0f);

	m_nowHp = resultHp_;

	// しっかり動作していることを確認
	if (m_nowHp <= 0.0f)
	{
		if (auto owner_ = GetOwner())
		{
			owner_->SetIsDeleteRequested(true);
		}
	}
}

void CombatComponent::LateUpdate()
{
	// 攻撃を始め出したら即座に敵に攻撃が当たるようにフラグを初期化
	auto componentObserverCache_ = m_componentObserverCache.lock();
	if (!componentObserverCache_) { return; }
	
	// 攻撃し始めたら当たり判定を有効化
	const bool isMatching_ = componentObserverCache_->IsEventMatching(StaticID<ComponentEventTagBase>::GetTypeID<AtkComboStartActiveTag>() , CommonEnum::EventLane::Moment, CommonEnum::BoolFlag::True);
	if (isMatching_)
	{
		m_hasAttackHit = false;
	}
}

void CombatComponent::OnCollision(const std::list<KdCollider::CollisionResult>& Result)
{
	if (Result.empty()) { return; }
	// 攻撃判定が出ていても攻撃が敵に当たっていたら当たり判定を消す
	if (m_hasAttackHit) { return; }
	
	bool isThrow = false;

	for (const auto& result_ : Result)
	{
		if (!BitShiftUtility::IsStandFlag(KdCollider::Type::TypeDamage, result_.m_collisionType)) { continue; }

		auto gameObject_ = result_.m_hitGameObject.lock();
		if (!gameObject_) { continue; }

		auto combatComponent_ = gameObject_->GetComponent<CombatComponent>().lock();
		if (!combatComponent_) { continue; }

		combatComponent_->TakeDamage(m_atkPow);
		isThrow = true;
	}

	// もし敵にダメージを与えていたら攻撃モーションが終わるまでダメージを与えられないようにする
	if (isThrow)
	{
		m_hasAttackHit = true;
	}
}

void CombatComponent::ImGuiPrefabDataInspector()
{
	ImGui::Text("HasAttackHit : %s" , ImGuiUtility::BoolToString(m_hasAttackHit));

	ImGui::DragFloat("AtkPow" , &m_atkPow , 0.1f);
	ImGui::DragFloat("DefPow" , &m_defPow , 0.1f);
	ImGui::DragFloat("NowHp"  , &m_nowHp  , 0.1f);
	ImGui::DragFloat("MaxHp"  , &m_maxHp  , 0.1f);

#ifdef _DEBUG
	ImGuiUtility::TagIDRadioButtonSelector("ComponentEventTag" , m_debugAddEventTag);
#endif 

#ifdef _DEBUG
	// 攻撃タイプに応じた攻撃ステータスの追加
	if (ImGui::Button("Add AtkType"))
	{
		CombatComponent::AtkStatus atkStatus_ = {};
		m_atkStatusList.emplace(m_debugAddEventTag , atkStatus_);
	}
#endif

	for (auto& [key_ , list_] : m_atkStatusList)
	{
		ImGui::DragFloat("Multiplier" , &list_.multiplier , 0.1f);
	}

	// ボタンが押されたら削除
	{
		auto itr_ = m_atkStatusList.begin();
		while (itr_ != m_atkStatusList.end())
		{
			if (ImGui::Button("Delete"))
			{
				itr_ = m_atkStatusList.erase(itr_);
			}
			else
			{
				itr_++;
			}
		}
	}
}

void CombatComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

	m_atkPow = Json.value("AtkPow" , CommonConstant::FLOAT_ONE);
	m_defPow = Json.value("DefPow" , CommonConstant::FLOAT_ONE);
	m_nowHp  = Json.value("NowHp"  , CommonConstant::FLOAT_ONE);
	m_maxHp  = Json.value("MaxHp"  , CommonConstant::FLOAT_ONE);

	if (JsonUtility::CheckIsContainAndArray(Json , "AtkStatusList"))
	{
		for (const auto& json_ : Json["AtkStatusList"])
		{
			const std::string eventTagName_ = json_.value("EventTagName" , CommonConstant::STRING_UNKNOWN);
			const uint32_t tagID_ = tagRegistry_.GetTagID(eventTagName_);
			
			CombatComponent::AtkStatus atkStatus = {};

			atkStatus.multiplier = json_.value("Multiplier" , CommonConstant::FLOAT_ONE);
			
			m_atkStatusList.emplace(tagID_ , atkStatus);
		}
	}
}

nlohmann::json CombatComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["AtkPow"] = m_atkPow;
	json_["DefPow"] = m_defPow;
	json_["NowHp" ] = m_nowHp;
	json_["MaxHp" ] = m_maxHp;

	// リストの中身を全てシリアライズ
	{
		auto rootJson_ = nlohmann::json::array();

		for (const auto& [key_ , value_] : m_atkStatusList)
		{
			auto atkJson_ = nlohmann::json();

			atkJson_["AtkType"	 ] = static_cast<int>(key_);
			atkJson_["Multiplier"] = value_.multiplier;
			
			rootJson_.emplace_back(atkJson_);
		}

		json_["AtkStatusList"] = rootJson_;
	}

	return json_;
}