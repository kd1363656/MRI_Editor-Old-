#include "RenderAnimationModelClip.h"

#include "../../../../../Utility/ImGui/ImGuiUtility.h"

#include "../../../../../CommonCore/Interpolator/CommonCoreInterpolator.h"

#include "../../../../../Modifier/Interporator/InterpolatorModifierBase.h"

void RenderAnimationModelClip::Init()
{
	// アニメーションの再生速度に絶対に必要
	if (!m_animationPlaySpeedInterpolator)
	{
		m_animationPlaySpeedInterpolator = std::make_shared<CommonCoreInterpolator>();
	}
	m_animationPlaySpeedInterpolator->Init();

	m_animationName = CommonConstant::STRING_UNKNOWN;

	m_isLoop = false;
}

void RenderAnimationModelClip::UpdateAnimationPlaySpeed()
{
	if (!m_animationPlaySpeedInterpolator) { return; }

	auto interpolator_ = m_animationPlaySpeedInterpolator->GetInterpolatorModifier().classData;
	if (!interpolator_) { return; }

	interpolator_->Update();
}

void RenderAnimationModelClip::ResetAnimationPlaySpeed()
{
	if (!m_animationPlaySpeedInterpolator) { return; }

	auto interpolator_ = m_animationPlaySpeedInterpolator->GetInterpolatorModifier().classData;
	if (!interpolator_) { return; }

	interpolator_->ResetInterpolate();
}

void RenderAnimationModelClip::ImGuiPrefabDataInspector()
{
	// ※注意 : アニメーション名は外部で設定
	if (ImGui::Button("Add AnimationTransition"))
	{
		CommonStruct::AnimationTransition animationTransition_ = CommonStruct::AnimationTransition();
		m_animationTransitionList.emplace_back(animationTransition_);
	}

	// リストを拡張する目的で作成
	// 具体的なクラスの種類はセレクターから選ぶ
	if (ImGui::Button("Add AnimationNotify"))
	{
		auto& notifyToComponentFactory_ = NotifyToComponentFactory::GetInstance();

		std::shared_ptr<NotifyToComponentBase> notify_ = notifyToComponentFactory_.Create(m_addAnimationNotifyToComponentName);
		m_animationNotifyToComponentList.emplace_back(notify_);
	}
	
	ImGuiAnimationTransitionList	   ();
	ImGuiAnimationNotifyToComponentList();

	// アニメーション再生速度補完の表示
	if (ImGui::TreeNode("AnimationPlaySpeedInterpolator"))
	{
		if (m_animationPlaySpeedInterpolator)
		{
			auto modifierCache_ = m_animationPlaySpeedInterpolator->GetInterpolatorModifier().classData;
			if (modifierCache_)
			{
				ImGui::Text("AnimationElapsedTime : %.2f", modifierCache_->GetCurrentValue());
			}
		}

		if (m_animationPlaySpeedInterpolator)
		{
			m_animationPlaySpeedInterpolator->ImGuiPrefabDataInspector();
		}

		ImGui::TreePop();
	}


	ImGui::Checkbox("IsLoop" , &m_isLoop);
}

void RenderAnimationModelClip::DeserializePrefabData(const nlohmann::json& Json) 
{
	if (Json.is_null()) { return; }

	const auto& notifyToComponentFactory_ = NotifyToComponentFactory::GetInstance();

	if (m_animationPlaySpeedInterpolator)
	{
		m_animationPlaySpeedInterpolator->DeserializePrefabData(Json);
	}

	// 再生中のアニメーションの次の遷移アニメーション情報を登録
	if (JsonUtility::CheckIsContainAndArray(Json , "AnimationTransitionList"))
	{
		for (const auto& animationTransition_ : Json["AnimationTransitionList"])
		{
			CommonStruct::AnimationTransition transition_ = {};

			transition_.condition.eventLane = JsonUtility::JsonToEnum<int>(CommonEnum::EventLane::None , animationTransition_ , "EventLane" );
			
			transition_.condition.eventTagID = JsonUtility::JsonToComponentEventTagID(animationTransition_ , "EventTagName");

			transition_.condition.notifyFlag  = JsonUtility::JsonToEnum<int>(CommonEnum::BoolFlag::None , animationTransition_ , "NotifyFlag");
			transition_.condition.triggerTime = animationTransition_.value("TriggerTime" , 0.0f);

			transition_.targetAnimationName = animationTransition_.value("TargetAnimationName" , CommonConstant::STRING_UNKNOWN);
			transition_.priority            = JsonUtility::JsonToEnum<int>(CommonEnum::Priority::None , animationTransition_, "Priority");
			
			transition_.isWaitForEnd = animationTransition_.value("IsWaitForEnd" , false);

			m_animationTransitionList.emplace_back(transition_);
		}
	}

	// アニメーション再生中にオブザーバーに伝える情報を選択
	if (JsonUtility::CheckIsContainAndArray(Json , "AnimationNotifyToComponentList"))
	{
		for (const auto& animationNotify_ : Json["AnimationNotifyToComponentList"])
		{
			if (animationNotify_.is_null()) { continue; }

			CommonStruct::GenericClassData<NotifyToComponentBase> notifyToComponentData_ = {};

			// 文字列とファクトトリーを用いてインスタンス化
			notifyToComponentData_.name      = animationNotify_.value("AnimationNotifyToComponentName" , CommonConstant::STRING_UNKNOWN);
			notifyToComponentData_.classData = notifyToComponentFactory_.Create(notifyToComponentData_.name);

			// もし文字列があったとしても生成できるクラスがなければ"return"
			if (!notifyToComponentData_.classData) { continue; }

			notifyToComponentData_.classData->Init();
			notifyToComponentData_.classData->SetComponentObserverCache(m_componentObserverCache);
			notifyToComponentData_.classData->DeserializeData          (animationNotify_);

			m_animationNotifyToComponentList.emplace_back(notifyToComponentData_);
		}
	}

	m_animationName = Json.value("AnimationName" , CommonConstant::STRING_UNKNOWN);

	m_isLoop = Json.value("IsLoop" , false);
}

nlohmann::json RenderAnimationModelClip::SerializePrefabData()
{
	auto rootJson_ = nlohmann::json();
	
	// 再生速度補完に関する情報をシリアライズ
	if (m_animationPlaySpeedInterpolator)
	{
		auto interpolatorJson_ = m_animationPlaySpeedInterpolator->SerializePrefabData();
		if (!interpolatorJson_.is_null())
		{
			rootJson_.update(interpolatorJson_);
		}
	}

	// 次の遷移アニメーション情報を保存
	auto animationTransitionListJson_ = nlohmann::json::array();
	for (const auto& animationTransition_ : m_animationTransitionList)
	{
		auto transitionJson_ = nlohmann::json();
		
		JsonUtility::ComponentEventTagIDToJson(transitionJson_ , animationTransition_.condition.eventTagID , "EventTagName");
		transitionJson_["EventLane"   ] = static_cast<int>(animationTransition_.condition.eventLane  );
		transitionJson_["NotifyFlag"  ] = static_cast<int>(animationTransition_.condition.notifyFlag );
		transitionJson_["TriggerTime" ] = animationTransition_.condition.triggerTime;

		transitionJson_["TargetAnimationName"] = animationTransition_.targetAnimationName;
		transitionJson_["Priority"           ] = static_cast<int>(animationTransition_.priority);

		transitionJson_["IsWaitForEnd"] = animationTransition_.isWaitForEnd;

		animationTransitionListJson_.emplace_back(transitionJson_);
	}

	// 受け取るオブザーバーの設定を保存
	auto animationNotifyListJson_ = nlohmann::json::array();
	for (const auto& animationNotifyToComponent : m_animationNotifyToComponentList)
	{
		// インスタンス化されていなければ処理を飛ばす
		if (!animationNotifyToComponent.classData) { continue; }
		auto notifyJson_ = nlohmann::json();

		notifyJson_["AnimationNotifyToComponentName"] = animationNotifyToComponent.name;
		notifyJson_.update(animationNotifyToComponent.classData->SerializeData());

		animationNotifyListJson_.emplace_back(notifyJson_);
	}

	rootJson_["AnimationTransitionList"       ] = animationTransitionListJson_;
	rootJson_["AnimationNotifyToComponentList"] = animationNotifyListJson_;

	rootJson_["AnimationName"] = m_animationName;
	
	rootJson_["IsLoop"] = m_isLoop;

	return rootJson_;
}

float RenderAnimationModelClip::GetAnimationPlaySpeed() const
{
	// ヌルチェックを行いインスタンスがあれば戻り値を返す
	if (!m_animationPlaySpeedInterpolator) { return 0.0f; }

	auto interpolator_ = m_animationPlaySpeedInterpolator->GetInterpolatorModifier();
	if (!interpolator_.classData) { return 0.0f; }

	return interpolator_.classData->GetCurrentValue();
}

void RenderAnimationModelClip::ImGuiAnimationTransitionList()
{
	auto modelWorkCache_ = m_modelWorkCache.lock();
	if (!modelWorkCache_) { return; }

	if (m_animationTransitionList.empty()) { return; }

	if (ImGui::TreeNode("AnimationTransitionList"))
	{
		auto itr_ = m_animationTransitionList.begin();
		while (itr_ != m_animationTransitionList.end())
		{
			auto& animationTransition_ = *itr_;
			
			ImGuiUtility::DrawSeparate();
			// イテレーターのポインタを取得
			ImGui::PushID(std::to_address(itr_));
			
			if (ImGui::Button("Delete"))
			{
				itr_ = m_animationTransitionList.erase(itr_);
				ImGui::PopID();
				continue;
			}

			ImGuiUtility::EnumRadioButtonSelector ("EventLane" , animationTransition_.condition.eventLane, ImGuiUtility::EVENT_LANE_TYPE_LIST);
			ImGuiUtility::TagIDRadioButtonSelector("EventTagID", animationTransition_.condition.eventTagID);
			ImGuiUtility::EnumRadioButtonSelector ("NotifyFlag", animationTransition_.condition.notifyFlag, ImGuiUtility::EVENT_NOTIFY_FLAG_LIST);

			const char* label_ = "TargetAnimation";
			if (ImGui::BeginCombo(label_, label_))
			{
				// モデルに存在するアニメーションの名前を表示して
				// 自身のアニメーションリストに追加する
				for (const auto& animation_ : *modelWorkCache_->GetAnimations())
				{
					if (!animation_) { continue; }

					// 重複するアニメーション名は登録できないようにするs
					bool isSelected_ = animationTransition_.targetAnimationName == animation_->m_name;

					if (ImGui::RadioButton(animation_->m_name.c_str(), isSelected_))
					{
						animationTransition_.targetAnimationName = animation_->m_name;
					}
				}

				ImGui::EndCombo();
			}

			// モデル情報からアニメーションの最大再生時間を取得してオブザーバーに上限値を伝える
			if (modelWorkCache_->GetAnimation(m_animationName))
			{
				const float max_ = modelWorkCache_->GetAnimation(m_animationName)->m_maxLength;
				ImGui::DragFloat("TriggerTime", &animationTransition_.condition.triggerTime, 0.1f, 0.0f, max_);
			}

			ImGuiUtility::EnumRadioButtonSelector("Priority", animationTransition_.priority, ImGuiUtility::PRIORITY_LIST);
			ImGui::Checkbox("IsWaitForEnd", &animationTransition_.isWaitForEnd);

			ImGui::PopID();

			itr_++;
		}

		ImGui::TreePop();
	}
}
void RenderAnimationModelClip::ImGuiAnimationNotifyToComponentList()
{
	auto modelWorkCache_ = m_modelWorkCache.lock();
	if (!modelWorkCache_) { return; }

	if (m_animationNotifyToComponentList.empty()) { return; }

	// アニメーションでオブザーバーを受け付ける時間
	if (ImGui::TreeNode("AnimationNotifyToComponentList"))
	{
		auto itr_ = m_animationNotifyToComponentList.begin();
		while (itr_ != m_animationNotifyToComponentList.end())
		{
			auto& notify_ = *itr_;

			ImGuiUtility::DrawSeparate();
			ImGui::PushID(std::to_address(itr_));

			// 選択されたらファクトリーからインスタンス化
			bool isThrow_ = ImGuiUtility::NotifyToComponentTypeSelector("Selector", notify_.name);
			if (isThrow_)
			{
				notify_.classData = NotifyToComponentFactory::GetInstance().Create(notify_.name);

				// しっかりインスタンス化されたなら初期化
				if (notify_.classData)
				{
					notify_.classData->Init();
					notify_.classData->SetComponentObserverCache(m_componentObserverCache);
				}
			}

			// もしインスタンスがなければイテレーターを更新して"continue"
			if (!notify_.classData) 
			{
				ImGui::PopID();
				itr_++;
				continue; 
			}

			if (ImGui::Button("Delete"))
			{
				ImGui::PopID();
				itr_ = m_animationNotifyToComponentList.erase(itr_);
				continue;
			}

			if (auto animationData_ = modelWorkCache_->GetAnimation(m_animationName))
			{
				notify_.classData->ImGuiInspector(animationData_->m_maxLength);
			}

			ImGui::PopID();

			itr_++;
		}

		ImGui::TreePop();
	}
}