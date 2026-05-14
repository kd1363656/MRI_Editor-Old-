#pragma once
#include "../../Model/RenderModelComponentBase.h"

class RenderAnimationModelClip;

class RenderAnimationModelComponent : public RenderModelComponentBase
{

public:

	RenderAnimationModelComponent ()          = default;
	~RenderAnimationModelComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<RenderAnimationModelComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Draw(const CommonEnum::StandardShaderTypeFlags Flag) override;

	void DispatchAnimationNotify();
	
	void OnPostCollisionResolved() override;

	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	bool IsInFrustum() const override;

	std::weak_ptr<KdModelWork> GetModelWork() const { return m_modelWork; }

	Math::Matrix GetBoneWorldMatrix(const CommonEnum::BoneType BoneType) override;

	const std::weak_ptr<RenderAnimationModelClip> GetAnimationClip (std::string_view AnimationName) const;

private:

	void LoadModel();

	void CacheCandidateNextAnimation();
	
	void ChangeAnimation(std::string_view AnimationName);
	void ChangeAnimation();
	
	std::shared_ptr<KdModelWork> m_modelWork = nullptr;
	std::shared_ptr<KdAnimator>  m_animator  = nullptr;

	std::unordered_map<std::string , std::shared_ptr<RenderAnimationModelClip> , CommonStruct::StringHash, std::equal_to<>> m_animationClipList;

	std::vector<CommonStruct::AnimationTransition*>     m_animationTransitionCacheList;
	
	std::weak_ptr<RenderAnimationModelClip> m_nowAnimationClip;

	std::string m_defaultAnimationClipName = CommonConstant::STRING_UNKNOWN;

	std::string m_debugAddAnimationName				 = "";
	std::string m_debugAddAnimationNotifyToComponent = "";
};