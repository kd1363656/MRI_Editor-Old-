#include "MoveInputActionComponent.h"

#include "../../../../Observer/Component/ComponentObserver.h"
#include "../../../../Tag/ComponentEvent/ComponentEventTag.h"
#include "../../../../Tag/TagRegistry.h"

#include "../../../../CommonCore/Move/CommonCoreMove.h"
#include "../../../../CommonCore/Move/Input/CommonCoreMoveInput.h"

#include "../../../../Modifier/Interporator/InterpolatorModifierBase.h"
#include "../../../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

#include "../../../Transform/TransformComponent.h"

#include "../../../../Utility/ImGui/ImGuiUtility.h"
#include "../../../../Utility/Json/JsonUtility.h"

void MoveInputActionComponent::Init()
{
	if (!m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput = std::make_shared<CommonCoreMoveInput>();
	}

	m_commonCoreMoveInput->Init();
}
void MoveInputActionComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput->PostLoadInit(owner_);
	}

	if (auto componentObserver_ = owner_->GetComponentObserver().lock())
	{
		m_componentObserverCache = componentObserver_;
	}
}

void MoveInputActionComponent::Update()
{
	auto componentObserverCache_ = m_componentObserverCache.lock();
	if (!componentObserverCache_) { return; }

	if (!m_commonCoreMoveInput) { return; }

	auto moveCache_ = m_commonCoreMoveInput->GetCommonCoreMoveCache().lock();
	if (!moveCache_) { return; }

	auto selfTransformComponentCache_ = moveCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	Math::Vector3 inputMoveDirection_ = m_commonCoreMoveInput->GetInputMoveDirection();

	// もし条件に一致したら移動速度のパラメーターをキャッシュする
	for (const auto& list_ : m_moveInputActionDataList)
	{
		if (!list_.interpolatorModifier.classData) { continue; }

		const bool isMatching_ = componentObserverCache_->IsEventMatching(list_.tagID , CommonEnum::EventLane::Keep , CommonEnum::BoolFlag::True);
		if (!isMatching_) 
		{
			// オブザーバーの通知を受け取らなければ移動加速度の初期化
			list_.interpolatorModifier.classData->ResetInterpolate();
			continue; 
		}

		m_nowInterpolatorModifierCache = list_.interpolatorModifier.classData;
		
		// 移動方向が"Json"で保存されていなければ入力方向に依存して動く
		if (list_.assignedMoveDirection)
		{
			inputMoveDirection_ = *list_.assignedMoveDirection;
		}
	}

	// 回転方向が指定されていなければ移動しない
	if (inputMoveDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }

	// 一回もオブザーバーの情報を受け取らなければ"return"
	if (m_nowInterpolatorModifierCache.expired()) { return; }

	auto interpolatorModifierBaseCache = m_nowInterpolatorModifierCache.lock();
	if (!interpolatorModifierBaseCache) { return; }

	Math::Matrix rotationMat_ = selfTransformComponentCache_->CreateRotationMatrix();
	inputMoveDirection_.Normalize();

	// 行列を使って移動ベクトルを回転
	Math::Vector3 rotateMoveDirection_ = Math::Vector3::TransformNormal(inputMoveDirection_, rotationMat_);
	// "Y"座標は移動しない
	rotateMoveDirection_.y = 0.0f;

	rotateMoveDirection_.Normalize();

	const float   deltaTime_ = Application::Instance().GetScaledDeltaTime();
	Math::Vector3 resultMove_ = rotateMoveDirection_ * interpolatorModifierBaseCache->GetCurrentValue() * deltaTime_;

	// 自身の"Transform"コンポーネントがあれば移動量を足しこんで座標を確定
	const Math::Vector3 resultPos_ = selfTransformComponentCache_->GetPos() + resultMove_;
	selfTransformComponentCache_->SetPos(resultPos_);

	// 移動速度を加速
	interpolatorModifierBaseCache->Update();

	// キャッシュを破棄
	m_nowInterpolatorModifierCache.reset();
}

void MoveInputActionComponent::ImGuiPrefabDataInspector()
{
	auto& observerRegistry_            = TagRegistry<ComponentEventTagBase>::GetInstance();
	auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance       ();

	ImGuiUtility::TagIDRadioButtonSelector("Add MoveInputActionData TagKey" , m_debugAddEventTag);

	if (ImGui::Button("Add MoveInputActionData"))
	{
		CommonStruct::MoveInputActionData moveDependOnAnimationData_ = {};
	
		// 移動量の加速情報は絶対に扱うのでインスタンス化しておく
		if (!moveDependOnAnimationData_.interpolatorModifier.classData)
		{
			moveDependOnAnimationData_.interpolatorModifier.classData = interpolatorModifierFactory_.Create(moveDependOnAnimationData_.interpolatorModifier.name);
		}

		// タグ"ID"だけ格納
		moveDependOnAnimationData_.tagID = m_debugAddEventTag;

		// 空の"MoveAnimationData"を格納
		RegistryMoveInputActionData(moveDependOnAnimationData_);
	}
	ImGui::Separator();

	for (auto& moveDependOnAnimationData_: m_moveInputActionDataList)
	{
		ImGui::PushID(&moveDependOnAnimationData_);
		if (ImGui::TreeNode(observerRegistry_.GetTagName(moveDependOnAnimationData_.tagID).c_str()))
		{
			if (ImGui::TreeNode("AssignedMoveDirection"))
			{
				const std::string tagName_ = observerRegistry_.GetTagName(moveDependOnAnimationData_.tagID);
				ImGui::Text(tagName_.c_str());

				ImGui::SameLine();
				// あらかじめ設定した移動方向か入力による移動方向かを決める
				if (!moveDependOnAnimationData_.assignedMoveDirection)
				{
					if (ImGui::Button("Use"))
					{
						moveDependOnAnimationData_.assignedMoveDirection = std::make_shared<Math::Vector3>();
					}
				}
				else
				{
					if (ImGui::Button("Don't Use"))
					{
						moveDependOnAnimationData_.assignedMoveDirection = nullptr;
					}
				}

				// 進む方向を決める(ゲームオブジェクトの前方向依存)
				if (moveDependOnAnimationData_.assignedMoveDirection)
				{
					ImGuiUtility::DirectionComboSelector("Direction", *moveDependOnAnimationData_.assignedMoveDirection);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("InterpolatorModifier"))
			{
				ImGuiUtility::InterpolatorModifierSelector(moveDependOnAnimationData_.interpolatorModifier.name.c_str(), moveDependOnAnimationData_.interpolatorModifier);
				
				if (moveDependOnAnimationData_.interpolatorModifier.classData)
				{
					moveDependOnAnimationData_.interpolatorModifier.classData->ImGuiInspector();
				}
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	if (ImGui::TreeNode("Delete"))
	{
		auto itr_ = m_moveInputActionDataList.begin();

		while (itr_ != m_moveInputActionDataList.end())
		{
			const std::string tagName_ = observerRegistry_.GetTagName(itr_->tagID);

			ImGui::PushID(&tagName_);

			if (ImGui::Button(tagName_.c_str()))
			{
				itr_ = m_moveInputActionDataList.erase(itr_);
			}
			else
			{
				itr_++;
			}

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

void MoveInputActionComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance();

	if (JsonUtility::CheckIsContainAndArray(Json , "MoveActionDataList"))
	{
		for (const auto& jsonArray_ : Json["MoveActionDataList"])
		{
			CommonStruct::MoveInputActionData moveDependOnAnimationData_ = {};

			moveDependOnAnimationData_.interpolatorModifier.name = jsonArray_.value("InterpolatorModifierName" , CommonConstant::STRING_UNKNOWN);

			if (!moveDependOnAnimationData_.interpolatorModifier.classData)
			{
				moveDependOnAnimationData_.interpolatorModifier.classData = interpolatorModifierFactory_.Create(moveDependOnAnimationData_.interpolatorModifier.name);
			}

			if (moveDependOnAnimationData_.interpolatorModifier.classData)
			{
				moveDependOnAnimationData_.interpolatorModifier.classData->Init();
				moveDependOnAnimationData_.interpolatorModifier.classData->DeserializeData(jsonArray_);
			}

			moveDependOnAnimationData_.tagID = JsonUtility::JsonToComponentEventTagID(jsonArray_ , "EventTagName");

			// 絶対に入力による移動方向を決めるのではなく設定された移動方向を決める場合があるので
			// "Json"ファイルに保存されていたら使用
			if (jsonArray_.contains("AssignedMoveDirection"))
			{
				moveDependOnAnimationData_.assignedMoveDirection  = std::make_shared<Math::Vector3>();
				*moveDependOnAnimationData_.assignedMoveDirection = JsonUtility::JsonToVec3(jsonArray_["AssignedMoveDirection"]);
			}

			// 読み込んだ情報をもとに登録
			RegistryMoveInputActionData(moveDependOnAnimationData_);
		}
	}	
}

nlohmann::json MoveInputActionComponent::SerializePrefabData()
{
	auto rootJson_  = nlohmann::json       ();
	auto arrayJson_ = nlohmann::json::array();

	// もしインスタンス化されていたら"Json"に保存
	// こうすることで移動方向が入力に依存するか指定された方向に依存するようになる
	for (auto& moveDependOnAnimationData_ : m_moveInputActionDataList)
	{
		auto json_ = nlohmann::json();

		JsonUtility::ComponentEventTagIDToJson(json_, moveDependOnAnimationData_.tagID , "EventTagName");

		if (moveDependOnAnimationData_.assignedMoveDirection)
		{
			json_["AssignedMoveDirection"] = JsonUtility::Vec3ToJson(*moveDependOnAnimationData_.assignedMoveDirection);
		}

		if (moveDependOnAnimationData_.interpolatorModifier.classData)
		{
			auto interpolatorModifierJson_ = moveDependOnAnimationData_.interpolatorModifier.classData->SerializeData();

			if (!interpolatorModifierJson_.is_null())
			{
				json_.update(interpolatorModifierJson_);
			}

			json_["InterpolatorModifierName"] = moveDependOnAnimationData_.interpolatorModifier.name;
		}

		arrayJson_.emplace_back(json_);
	}

	rootJson_["MoveActionDataList"] = arrayJson_;

	return rootJson_;
}

void MoveInputActionComponent::RegistryMoveInputActionData(const CommonStruct::MoveInputActionData& MoveInputActionData)
{
	// 無効な"ID"を登録しない
	if (MoveInputActionData.tagID == CommonConstant::INVALID_STATIC_ID) { return; }

	// もしも同じ"ID"がリスト内に存在すれば追加しない
	for (const auto& moveDependOnAnimationData_ : m_moveInputActionDataList)
	{
		if (moveDependOnAnimationData_.tagID == MoveInputActionData.tagID) { return; }
	}

	m_moveInputActionDataList.emplace_back(MoveInputActionData);
}