#include "RenderAnimationModelComponent.h"
#include "../../../Transform/TransformComponent.h"

#include "../../../../CommonCore/Render/CommonCoreRender.h"
#include "../../../../CommonCore/Render/Model/CommonCoreRenderModel.h"

#include "../../../../CommonCore/AssetFilePath/CommonCoreAssetFilePath.h"

#include "../../../../Observer/Component/ComponentObserver.h"
#include "../../../../Tag/ComponentEvent/ComponentEventTag.h"
#include "../../../../Tag/TagRegistry.h"

#include "../../../../Factory/Notify/Component/NotifyToComponentFactory.h"

#include "../../../../Utility/BitShift/BitShiftUtility.h"
#include "../../../../Utility/String/StringUtility.h"

#include "Clip/RenderAnimationModelClip.h"

void RenderAnimationModelComponent::Init()
{
	RenderModelComponentBase::Init();

	if (!m_modelWork)
	{
		m_modelWork = std::make_shared<KdModelWork>();
	}

	if (!m_animator)
	{
		m_animator = std::make_shared<KdAnimator>();
	}
}
void RenderAnimationModelComponent::PostLoadInit()
{
	RenderModelComponentBase::PostLoadInit();

	// デフォルトのアニメーションをセット
	ChangeAnimation(m_defaultAnimationClipName);
}

void RenderAnimationModelComponent::Draw(const CommonEnum::StandardShaderTypeFlags Flag)
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();

	if (!commonCoreRenderModelCache_)																    { return; }
	if (!BitShiftUtility::IsStandFlag(Flag, commonCoreRenderModelCache_->GetStandardShaderTypeFlags())) { return; }

	auto commonCoreRenderCache_ =commonCoreRenderModelCache_->GetCommonCoreRenderCache().lock();
	if (!commonCoreRenderCache_) { return; }

	auto selfTransformComponentCache_ = commonCoreRenderCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	// シェーダエフェクトの有効化
	for (const auto& renderShaderParam_ : GetRenderShaderParamList())
	{
		if (!renderShaderParam_.classData) { continue; }
		renderShaderParam_.classData->EnableParamEffect();
	}

	if (m_modelWork)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel
		(
			*m_modelWork								  ,
			selfTransformComponentCache_->GetMatrix() ,
			commonCoreRenderCache_->GetColor       ()
		);
	}

	// シェーダエフェクトの無効化
	for (const auto& renderShaderParam_ : GetRenderShaderParamList())
	{
		if (!renderShaderParam_.classData) { continue; }
		renderShaderParam_.classData->DisableParamEffect();
	}
}

void RenderAnimationModelComponent::DispatchAnimationNotify()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (!m_animator || !m_modelWork) { return; }

	// 現在キャッシュされているアニメーションになにも格納されていなければ"return"
	auto nowAnimation_ = m_nowAnimationClip.lock();
	if (!nowAnimation_) { return; }

	// アニメーションによる通知の制御
	for (auto& animationNotifyToComponent_ : nowAnimation_->GetWorkAnimationNotifyToComponentList())
	{
		if (!animationNotifyToComponent_.classData) { continue; }
		animationNotifyToComponent_.classData->DispatchNotify(m_animator->GetTime());
	}
}

void RenderAnimationModelComponent::OnPostCollisionResolved()
{
	if (!m_modelWork || !m_animator) { return; }

	const float deltaTime_        = Application::Instance().GetScaledDeltaTime();
	auto		nowAnimationClip_ = m_nowAnimationClip.lock();

	if (!nowAnimationClip_) { return; }

	// アニメーションの存在を確認してから更新
	if (m_modelWork->GetAnimation(nowAnimationClip_->GetAnimationName()))
	{
		m_animator->AdvanceTime(m_modelWork->WorkNodes(), nowAnimationClip_->GetAnimationPlaySpeed() * deltaTime_);
		m_modelWork->CalcNodeMatrices();
	}

	// アニメーションの再生が完了したら補完の更新を行う
	nowAnimationClip_->UpdateAnimationPlaySpeed();

	CacheCandidateNextAnimation();
	ChangeAnimation			   ();
}

void RenderAnimationModelComponent::ImGuiSpawnDataInspector()
{
	RenderModelComponentBase::ImGuiSpawnDataInspector();
}
void RenderAnimationModelComponent::ImGuiPrefabDataInspector()
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();
	if (!commonCoreRenderModelCache_) { return; }
	if (!m_modelWork || !m_animator ) { return; }

	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto observer_ = owner_->GetComponentObserver().lock();
	if (!observer_) { return; }
	
	RenderModelComponentBase::ImGuiPrefabDataInspector();

	if (ImGui::CollapsingHeader("AnimationClip"))
	{
		ImGuiUtility::DrawSeparate();
		{
			// アニメーション名を選ぶ
			const char* label_ = "AnimationName";
			if (ImGui::BeginCombo(label_, label_))
			{
				// モデルに存在するアニメーションの名前を表示して
				// 自身のアニメーションリストに追加する
				if (m_modelWork->GetAnimations())
				{
					for (const auto& animation_ : *m_modelWork->GetAnimations())
					{
						if (!animation_) { continue; }

						// 重複するアニメーション名は登録できないようにする
						bool isSelected_ = m_debugAddAnimationName == animation_->m_name;

						if (ImGui::RadioButton(animation_->m_name.c_str(), isSelected_))
						{
							m_debugAddAnimationName = animation_->m_name;
						}
					}
				}

				ImGui::EndCombo();
			}
		}

		// 選択した登録済みでないアニメーションを追加
		ImGui::SameLine();
		if (ImGui::Button("Add AnimationClip"))
		{
			bool isDuplicated_ = false;

			for (const auto& [animationClipName_, animationClip_] : m_animationClipList)
			{
				if (animationClipName_ != m_debugAddAnimationName) { continue; }
				isDuplicated_ = true;
			}

			if (!isDuplicated_)
			{
				std::shared_ptr<RenderAnimationModelClip> animationClip_ = std::make_shared<RenderAnimationModelClip>();
				animationClip_->Init					 ();
				animationClip_->SetAnimationName         (m_debugAddAnimationName);
				animationClip_->SetModelWorkCache        (m_modelWork);
				animationClip_->SetComponentObserverCache(observer_);
				m_animationClipList.try_emplace(m_debugAddAnimationName, animationClip_);
			}
		}

		// アニメーションクリップの詳細を表示
		if (ImGui::TreeNode("AnimationClipList"))
		{
			for (const auto& [animationClipName_, animationClip_] : m_animationClipList)
			{
				if (!animationClip_) { continue; }

				if (ImGui::CollapsingHeader(animationClipName_.c_str()))
				{
					// このボタンが押された瞬間アニメーションを切り替える
					std::string text_ = "Play : " + animationClipName_;

					if (ImGui::Button(text_.c_str()))
					{
						ChangeAnimation(animationClipName_);
					}

					animationClip_->ImGuiPrefabDataInspector();
				}
			}
			ImGui::TreePop();
		}

		// 保存されているアニメーションクリップからデフォルトのアニメーションを選択
		if (ImGui::TreeNode("SelectDefaultAnimation"))
		{
			ImGui::Text("DefaultAnimationClip : %s", m_defaultAnimationClipName.c_str());

			if (ImGui::BeginCombo(m_defaultAnimationClipName.c_str(), m_defaultAnimationClipName.c_str()))
			{
				for (auto& [key_, value_] : m_animationClipList)
				{
					if (!value_) { continue; }

					ImGui::PushID(&value_);

					bool isSelected_ = m_defaultAnimationClipName == key_;
					if (ImGui::RadioButton(key_.c_str(), &isSelected_))
					{
						// 選択されたアニメーションをデフォルト値として扱う
						m_defaultAnimationClipName = value_->GetAnimationName().data();
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Asset FilePath"))
	{
		// パスが変更されたらモデルを上書き
		if (auto commonCoreRender = commonCoreRenderModelCache_->GetCommonCoreRenderCache().lock())
		{
			if (auto commonCoreAssetFilePath = commonCoreRender->GetCommonCoreAssetFilePathCache().lock())
			{
				commonCoreAssetFilePath->ImGuiPrefabDataInspector();

				if (commonCoreAssetFilePath->GetHasPathChanged())
				{
					if (!m_modelWork)
					{
						m_modelWork = std::make_shared<KdModelWork>();
					}

					m_modelWork->SetModelData(commonCoreAssetFilePath->GetAssetFilePath());
				}
			}
		}
	}
}

void RenderAnimationModelComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	RenderModelComponentBase::DeserializeSpawnData(Json);
}
void RenderAnimationModelComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto observer_ = owner_->GetComponentObserver().lock();
	if (!observer_) { return; }

	RenderModelComponentBase::DeserializePrefabData(Json);

	// アニメーションクリップの読み込み
	if (JsonUtility::CheckIsContainAndArray(Json, "AnimationClipList"))
	{
		for (const auto& array_ : Json["AnimationClipList"])
		{
			std::string animationClipName_ = array_.value("AnimationClipName", CommonConstant::STRING_UNKNOWN);

			if (array_.contains("AnimationClip"))
			{
				std::shared_ptr<RenderAnimationModelClip> animationClip_ = std::make_shared<RenderAnimationModelClip>();

				// 各種必要なパラメータを格納
				animationClip_->Init();
				animationClip_->SetAnimationName		 (animationClipName_);
				animationClip_->SetComponentObserverCache(observer_);
				animationClip_->SetModelWorkCache        (m_modelWork);

				animationClip_->DeserializePrefabData(array_["AnimationClip"]);

				m_animationClipList.try_emplace(animationClipName_ , animationClip_);
			}
		}
	}

	// 通常時のアニメーションクリップの読み込み
	m_defaultAnimationClipName = Json.value("DefaultAnimationClipName", CommonConstant::STRING_UNKNOWN);

	// ファイルパスが格納されてからロード
	LoadModel();
}

nlohmann::json RenderAnimationModelComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	auto baseJson_ = RenderModelComponentBase::SerializeSpawnData();
	if (!baseJson_.is_null()) 
	{
		json_.update(baseJson_);
	}

	return json_;
}
nlohmann::json RenderAnimationModelComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	auto baseJson_ = RenderModelComponentBase::SerializePrefabData();
	if (!baseJson_.is_null())
	{
		json_.update(baseJson_);
	}

	{
		auto rootJson_ = nlohmann::json::array();

		for (const auto& [key_, value_] : m_animationClipList)
		{
			if (!value_) { continue; }
			auto animationClipJson_ = nlohmann::json();

			animationClipJson_["AnimationClipName"] = key_;
			animationClipJson_["AnimationClip"    ] = value_->SerializePrefabData();
			
			rootJson_.emplace_back(animationClipJson_);
		}

		json_["AnimationClipList"] = rootJson_;
	}

	json_["DefaultAnimationClipName"] = m_defaultAnimationClipName;

	return json_;
}

bool RenderAnimationModelComponent::IsInFrustum() const
{
	return true;
}

Math::Matrix RenderAnimationModelComponent::GetBoneWorldMatrix(const CommonEnum::BoneType BoneType)
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();
	if (!commonCoreRenderModelCache_) { return Math::Matrix::Identity; }

	if (!m_modelWork) { return Math::Matrix::Identity; }

	std::string boneName_ = commonCoreRenderModelCache_->GetBoneName(BoneType);

	auto node_ = m_modelWork->FindDataNode(boneName_);
	if (!node_) { return Math::Matrix::Identity; }

	return node_->m_worldTransform;
}

const std::weak_ptr<RenderAnimationModelClip> RenderAnimationModelComponent::GetAnimationClip(std::string_view AnimationName) const
{
	auto itr_ = m_animationClipList.find(AnimationName);
	if (itr_ != m_animationClipList.end())
	{
		return itr_->second;
	}

	return std::weak_ptr<RenderAnimationModelClip>();
}

void RenderAnimationModelComponent::LoadModel()
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();
	if (!commonCoreRenderModelCache_) { return; }

	auto render_ = commonCoreRenderModelCache_->GetCommonCoreRenderCache().lock();
	if (!render_) { return; }

	auto commonCoreAssetFilePath = render_->GetCommonCoreAssetFilePathCache().lock();

	// "json"ファイルが読み込まれた時点でモデルを読み込む(当たり判定などでモデル情報が必要なため)
	if (m_modelWork && commonCoreAssetFilePath)
	{
		if (!commonCoreAssetFilePath->GetAssetFilePath().empty())
		{
			m_modelWork->SetModelData(commonCoreAssetFilePath->GetAssetFilePath());
		}

		// "Idle"アニメーションを設定
		ChangeAnimation(m_defaultAnimationClipName);
	}
}

void RenderAnimationModelComponent::CacheCandidateNextAnimation()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto observer_ = owner_->GetComponentObserver().lock();
	if (!observer_) { return; }

	if (!m_modelWork || !m_animator) { return; }

	auto nowAnimationClip_ = m_nowAnimationClip.lock();
	if (!nowAnimationClip_) { return; }

	// 指定したオブザーバーの通知を受け取りアニメーションを変更するか判断する
	for (auto& transition_ : nowAnimationClip_->GetWorkAnimationTransitionList())
	{
		// もし指定したオブザーバーのイベントレーンのフラグと合致しなければ"continue"
		CommonEnum::BoolFlag flag_ = observer_->GetNotification(transition_.condition.eventTagID , transition_.condition.eventLane);
		if (flag_ != transition_.condition.notifyFlag) { continue; }

		// もし指定したアニメーション通知受け取り時間を過ぎたら受け取りを開始する
		if (m_animator->GetTime() <= transition_.condition.triggerTime) { continue; }

		// 何回もリストにキャッシュしないようにするためフラグで追加を管理
		if (!transition_.isTriggered)
		{
			transition_.isTriggered = true;
			m_animationTransitionCacheList.emplace_back(&transition_);
		}
	}
}

void RenderAnimationModelComponent::ChangeAnimation(std::string_view AnimationName)
{
	if (!m_animator || !m_modelWork) { return; }

	// もし現在再生中のアニメーションがなければアニメーションをセットして"return"
	auto nowAnimationClip_ = m_nowAnimationClip.lock();
	if (!nowAnimationClip_) 
	{
		auto animationClip_ = GetAnimationClip(AnimationName).lock();
		if (!animationClip_) { return; }
		
		m_nowAnimationClip = animationClip_;

		// 取得したアニメーションが存在すればアニメーションをアニメーターにセット
		auto obtainedAnimationClip_ = m_nowAnimationClip.lock();
		if (!obtainedAnimationClip_) { return; }

		// アニメーションをセット
		if (auto animation_ = m_modelWork->GetAnimation(obtainedAnimationClip_->GetAnimationName()))
		{
			m_animator->SetAnimation(animation_ , obtainedAnimationClip_->GetIsLoop());
		}
		return; 
	}

	// 今同じアニメーションが再生されていたらアニメーションを変えない
	if (AnimationName.data() == nowAnimationClip_->GetAnimationName()) { return; }

	// 読み取り専用ポインタ
	const std::shared_ptr<RenderAnimationModelClip> readOnly_ = GetAnimationClip(AnimationName).lock();
	if (!readOnly_) { return; }

	// アニメーションを切り替える前に今のアニメーションで通知していたイベントの通知をやめる
	for (auto& animationNotify_ : nowAnimationClip_->GetWorkAnimationNotifyToComponentList())
	{
		if (!animationNotify_.classData) { continue; }
		animationNotify_.classData->ResetNotify();
	}

	// 現在使用していたアニメーションの再生速度の初期化
	nowAnimationClip_->ResetAnimationPlaySpeed();

	// アニメーションを切り替えてイベント通知をする
	m_nowAnimationClip = readOnly_;

	auto obtainedAnimationClip_ = m_nowAnimationClip.lock();
	if (!obtainedAnimationClip_) { return; }

	// アニメーションをセット
	if (auto animation_ = m_modelWork->GetAnimation(obtainedAnimationClip_->GetAnimationName()))
	{
		m_animator->SetAnimation(animation_ , obtainedAnimationClip_->GetIsLoop());
	}
}
void RenderAnimationModelComponent::ChangeAnimation()
{
	// アニメーション切り替え
	const CommonStruct::AnimationTransition* onlyRead_ = nullptr;
	for (const auto& animationList_ : m_animationTransitionCacheList)
	{
		// 一応ヌルチェック
		if (!animationList_) { continue; }

		// "nullptr"なら初めてのアドレス格納なので情報を取り込む
		if (!onlyRead_)
		{
			onlyRead_ = animationList_;
		}

		if (!onlyRead_) { continue; }

		// "Priority"の値が小さければ小さいほど優先順位が高い
		if (static_cast<int>(onlyRead_->priority) > static_cast<int>(animationList_->priority))
		{
			onlyRead_ = animationList_;
		}
	}

	bool isThrow = false;

	// ポインタがアドレスを保持していたならそのアニメーションに変換
	if (onlyRead_)
	{
		// アニメーション終了までアニメーション切り替えを待つかどうか
		if (onlyRead_->isWaitForEnd)
		{
			if (m_animator->IsAnimationEnd())
			{
				ChangeAnimation(onlyRead_->targetAnimationName);
				isThrow = true;
			}
		}
		else
		{
			ChangeAnimation(onlyRead_->targetAnimationName);
			isThrow = true;
		}
		
		// オブザーバーから受け取った通知をリセット
		if (isThrow)
		{
			// 通知を受け取った情報をリセット
			for (auto& animationList_ : m_animationTransitionCacheList)
			{
				if (!animationList_) { continue; }

				animationList_->isTriggered = false;
			}

			m_animationTransitionCacheList.clear();
		}
	}

	// もしアニメーションの再生が終わっていてアニメーション遷移が行われていなければデフォルトアニメーションに戻る
	if (!isThrow && m_animator->IsAnimationEnd())
	{
		ChangeAnimation(m_defaultAnimationClipName);
	}
}