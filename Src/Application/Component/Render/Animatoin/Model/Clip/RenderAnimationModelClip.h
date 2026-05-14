#pragma once

class CommonCoreInterpolator;
class ComponentObserver;

class RenderAnimationModelClip
{

public:

	RenderAnimationModelClip () = default;
	~RenderAnimationModelClip() = default;

	void Init();

	void UpdateAnimationPlaySpeed();
	void ResetAnimationPlaySpeed ();

	void ImGuiPrefabDataInspector();

	void DeserializePrefabData(const nlohmann::json& Json);

	nlohmann::json SerializePrefabData();

	auto& GetWorkAnimationTransitionList		   () { return m_animationTransitionList;		 }
	auto& GetWorkAnimationNotifyToComponentList() { return m_animationNotifyToComponentList; }

	std::string_view GetAnimationName() const { return m_animationName; }

	float GetAnimationPlaySpeed() const;

	bool GetIsLoop() const { return m_isLoop; }

	void SetComponentObserverCache(std::weak_ptr<ComponentObserver> Set) { m_componentObserverCache = Set; }
	void SetModelWorkCache		  (std::weak_ptr<KdModelWork>       Set) { m_modelWorkCache			= Set; }

	void SetAnimationName(const std::string& Set) { m_animationName = Set; }

private:

	void ImGuiAnimationTransitionList       ();
	void ImGuiAnimationNotifyToComponentList();

	std::weak_ptr<KdModelWork>		 m_modelWorkCache;
	std::weak_ptr<ComponentObserver> m_componentObserverCache;

	// 要素を削除しない前提なので一番高速な"std::vector"を採用
	std::vector<CommonStruct::AnimationTransition>                     m_animationTransitionList;
	std::vector<CommonStruct::GenericClassData<NotifyToComponentBase>> m_animationNotifyToComponentList;
	std::shared_ptr<CommonCoreInterpolator>							   m_animationPlaySpeedInterpolator = nullptr;

	std::string m_animationName = CommonConstant::STRING_UNKNOWN;

	bool m_isLoop = false;

	std::string m_addAnimationNotifyToComponentName = CommonConstant::STRING_UNKNOWN;
};