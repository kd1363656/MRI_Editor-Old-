#pragma once
#include "../../Tag/TagRegistry.h"

class GameObject;

class ComponentObserver
{

public:

	ComponentObserver () = default;
	~ComponentObserver() = default;

	void BeginFrame();

	void RegisterEvent(const std::string& TagName);

	void ImGuiPrefabDataInspector();

	void DeserializePrefabData(const nlohmann::json& Json);

	nlohmann::json SerializePrefabData();

	void NotifyEvent(uint32_t EventTagID , CommonEnum::EventLane Lane, CommonEnum::BoolFlag Set);

	bool IsEventMatching(uint32_t EventTagID , CommonEnum::EventLane Lane , CommonEnum::BoolFlag WantResult);

	CommonStruct::EventTypeData GetEventData   (uint32_t ReceiveEventTagID) const;
	CommonEnum::BoolFlag		GetNotification(uint32_t ReceiveEventTagID , CommonEnum::EventLane EventLane) const;

private:

	std::unordered_map<uint32_t , CommonStruct::EventTypeData> m_eventList;

#ifdef _DEBUG
	std::string m_addTagName = CommonConstant::STRING_UNKNOWN;
#endif
};