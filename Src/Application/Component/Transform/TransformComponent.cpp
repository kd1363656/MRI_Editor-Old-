#include "TransformComponent.h"

#include "../../Utility/Json/JsonUtility.h"

#include "../../../System/Calculation/Calculation.h"
#include "../../Utility/ImGui/ImGuiUtility.h"
#include "../../Utility/BitShift/BitShiftUtility.h"

void TransformComponent::Init()
{
	m_transformData.pos         = Math::Vector3::Zero;
	m_transformData.rotation    = Math::Quaternion::Identity;
	m_transformData.scale       = Math::Vector3::One;
	m_transformData.worldMatrix = Math::Matrix::Identity;

	m_canUpdateMatrix       = true;
	m_isUpdateMatrixBlocked = false;
}
void TransformComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	// 親の"TransformComponent"をキャッシュ
	if (auto parent_ = owner_->GetParent().lock())
	{
		m_parentTransformComponent = parent_->GetComponent<TransformComponent>();
	}

	// 子の"TransformComponent"をキャッシュ
	for (const auto& list_ : owner_->GetChildList())
	{
		auto child_ = list_.lock();
		if (!child_) { continue; }

		// もし"TransformComponent"がなければ処理を飛ばす
		auto transformComponent_ = child_->GetComponent<TransformComponent>();
		if (transformComponent_.expired()) { continue; }

		m_childTransformComponent.emplace_back(transformComponent_);
	}
	
	const auto& strategyFactory_ = StrategyFactory<TransformComponent>::GetInstance();
	m_strategyData.strategy = strategyFactory_.Create(m_strategyData.name);

	// ストラテジーが確定したら一回だけ現在位置での行列を確定
	UpdateStrategy();
}

void TransformComponent::EarlyUpdate()
{
	// 衝突情報の初期化
	m_sphereCollisionResult = {};
	m_rayCollisionResult    = {};

	// ダーティーフラグを下げる
	m_canUpdateMatrix       = false;
	m_isUpdateMatrixBlocked = false;
}

void TransformComponent::OnCollision(const std::list<KdCollider::CollisionResult>& Result)
{
	for (const auto& list_ : Result)
	{
		// もし地形との当たり判定でなければ処理を飛ばす
		if (!BitShiftUtility::IsStandFlag(KdCollider::Type::TypeGround, list_.m_collisionType)) { continue; }

		// 重なり量が一番多いオブジェクトの情報を取得
		if (list_.m_shapeType == KdCollider::ShapeType::Sphere)
		{
			if (m_sphereCollisionResult.m_overlapDistance < list_.m_overlapDistance)
			{
				m_sphereCollisionResult = list_;
			}
		}
		if (list_.m_shapeType == KdCollider::ShapeType::Ray)
		{
			if (m_rayCollisionResult.m_overlapDistance < list_.m_overlapDistance)
			{
				m_rayCollisionResult = list_;		
			}
		}
	}
}
void TransformComponent::OnPostCollision()
{
	bool isHit_ = false;

	// ベクトル情報がなければヒットしていなかったことになる
	if (m_sphereCollisionResult.m_shapeType == KdCollider::ShapeType::Sphere)
	{
		Math::Vector3 bump_ = m_sphereCollisionResult.m_hitDir * m_sphereCollisionResult.m_overlapDistance;

		// 押し戻し距離分押し戻す
		bump_.y = 0.0f;

		// ワールド座標で行列をセット
		Math::Vector3 newWorldPos_ = GetWorldPos() + bump_;
		SetLocalPos(newWorldPos_);
	}
	if(m_rayCollisionResult.m_shapeType == KdCollider::ShapeType::Ray)
	{
		Math::Vector3 newWorldPos_ = GetWorldPos();
		newWorldPos_.y = m_rayCollisionResult.m_hitPos.y;

		SetLocalPos(newWorldPos_);
	}

}

void TransformComponent::UpdateStrategy()
{
	// ストラテジーが"Null"じゃなければ更新
	if (!m_strategyData.strategy) { return; }
	
	m_strategyData.strategy->Execute(shared_from_this());
}
void TransformComponent::FixMatrix()
{
	// もし親の行列が更新されたら自身の行列も更新する
	CheckCanMatrixUpdate();

	// 行列の更新が可能なら更新
	if (!m_canUpdateMatrix) { return; }
	
	UpdateStrategy();
}

void TransformComponent::ImGuiSpawnDataInspector()
{
	if (ImGui::DragFloat3("Pos" , &m_transformData.pos.x , 0.1f))
	{
		UpdateStrategy();
	}

	ImGui::Checkbox("CanUpdateMat" , &m_canUpdateMatrix);

	CommonImGuiInspector();
}
void TransformComponent::ImGuiPrefabDataInspector()
{
	CommonImGuiInspector();
}

void TransformComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (Json.contains("Pos")) 
	{
		m_transformData.pos = JsonUtility::JsonToVec3(Json["Pos"]);
	}
	if (Json.contains("Rotation")) 
	{
		m_transformData.rotation = JsonUtility::JsonToQuaternion(Json["Rotation"]);
	}
	if (Json.contains("Scale"))
	{
		m_transformData.scale = JsonUtility::JsonToVec3(Json["Scale"]);
	}

	m_strategyData.name = Json.value("StrategyName", CommonConstant::STRING_UNKNOWN);
}
void TransformComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_strategyData.name = Json.value("StrategyName", CommonConstant::STRING_UNKNOWN);
}

nlohmann::json TransformComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();
	
	json_["Pos"     ] = JsonUtility::Vec3ToJson      (m_transformData.pos     );
	json_["Rotation"] = JsonUtility::QuaternionToJson(m_transformData.rotation);
	json_["Scale"   ] = JsonUtility::Vec3ToJson      (m_transformData.scale   );

	json_["StrategyName"] = m_strategyData.name;

	return json_;
}
nlohmann::json TransformComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["StrategyName"] = m_strategyData.name;

	return json_;
}

Math::Matrix TransformComponent::CreateMatrix() const
{	
	const Math::Matrix mat_ = CreateScaleMatrix      () * 
							  CreateRotationMatrix   () *
							  CreateTranslationMatrix();
	return mat_;
}
Math::Matrix TransformComponent::GetWorldMatrix()
{
	// 現在の親子関係を加味したワールド行列を作成
	if (auto parent_ = m_parentTransformComponent.lock())
	{
		parent_->UpdateStrategy();
	}

	UpdateStrategy();

	return m_transformData.worldMatrix;
}

Math::Vector3 TransformComponent::GetWorldPos()
{
	// 現在の親子関係を加味したワールド行列を作成
	// 現在の親子関係を加味したワールド行列を作成
	if (auto parent_ = m_parentTransformComponent.lock())
	{
		parent_->UpdateStrategy();
	}

	UpdateStrategy();

	return m_transformData.worldMatrix.Translation();
}

void TransformComponent::SetMatrix(const Math::Matrix& Set)
{
	m_transformData.worldMatrix = Set;
	m_canUpdateMatrix		    = true;
}

void TransformComponent::SetRotation(const Math::Quaternion& Set)
{
	m_transformData.rotation = Set;
	m_canUpdateMatrix        = true;
}

void TransformComponent::SetPos(const Math::Vector3& Set)
{
	m_transformData.pos = Set;
	m_canUpdateMatrix   = true;
}
void TransformComponent::SetPosY(const float Set)
{
	m_transformData.pos.y = Set;
	m_canUpdateMatrix     = true;
}
void TransformComponent::SetWorldPos(const Math::Vector3& Set)
{
	m_transformData.worldMatrix.Translation(Set);
	m_canUpdateMatrix = false;
	// ここでの行列合成の結果を保持するため行列の合成を止めさせる(直接ワールド座標を指定するため)
	m_isUpdateMatrixBlocked = true;
}

void TransformComponent::SetLocalPos(const Math::Vector3& Convert)
{
	// 親が存在しなければそのまま座標をセット
	auto parent_ = m_parentTransformComponent.lock();
	if (!parent_) 
	{
		m_transformData.pos = Convert;
		return;
	}

	// ワールド座標をローカル座標に変換して格納
	// 親のワールド行列の逆行列を取得
	Math::Matrix parentInvert_ = Math::Matrix::Identity;
	parentInvert_ = parent_->GetWorkMatrix().Invert();

	Math::Matrix localTranslationMat_ = Math::Matrix::CreateTranslation(Convert) * parentInvert_;
	
	m_transformData.pos = localTranslationMat_.Translation();
}

void TransformComponent::CommonImGuiInspector()
{
	Math::Vector3 euler_ = Math::Vector3::Zero;

	// クオータニオンをオイラー角に変換
	Calculation::QuaternionToEuler(m_transformData.rotation , euler_);

	if (ImGui::DragFloat3("Rotation", &euler_.x , 1.0f , -CommonConstant::HALF_DEGREE , CommonConstant::HALF_DEGREE))
	{
		UpdateStrategy();
	}

	if(ImGui::DragFloat3("Scale", &m_transformData.scale.x, 0.1f))
	{
		UpdateStrategy();
	}

	// 変換したオイラー角の結果をクオータニオンに変換
	Calculation::EulerToQuaternion(euler_ , m_transformData.rotation);

	// 行列の合成方法の文字列を入れ込む
	ImGuiUtility::StrategySelector<TransformComponent>("MatrixStrategy", m_strategyData.name, m_strategyData.strategy);

	ImGui::Separator();

	{
		Math::Vector3 worldPos_ = GetWorldMatrix().Translation();

		ImGui::Text(U8("ワールド座標"));
		ImGui::Text("X : %.3f , Y : %.3f , Z : %.3f", worldPos_.x, worldPos_.y, worldPos_.z);
	}
}

void TransformComponent::CheckCanMatrixUpdate()
{
	// 親の"Transform"が更新されていればこちらも更新する(親から見た子であるから)
	if (auto parentCache_ = m_parentTransformComponent.lock())
	{
		if (parentCache_->GetCanUpdateMatrix())
		{
			m_canUpdateMatrix = true;
		}
	}

	// フラグが上がっていたら行列更新を今回のフレームで行わない
	if (m_isUpdateMatrixBlocked)
	{
		m_canUpdateMatrix = false;
	}
}