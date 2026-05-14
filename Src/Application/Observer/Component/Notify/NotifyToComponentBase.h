#pragma once

class ComponentObserver;

class NotifyToComponentBase
{

public:

	NotifyToComponentBase         () = default;
	virtual ~NotifyToComponentBase() = default;

	virtual void Init();

	virtual void DispatchNotify(float ElapsedTime) = 0;

	virtual void ResetNotify() = 0;

	virtual void ImGuiInspector(float NotifyTimeLimit);

	virtual void DeserializeData(const nlohmann::json Json);

	virtual nlohmann::json SerializeData() const;

	std::weak_ptr<ComponentObserver>& GetWorkComponentObserverCache() { return m_componentObserverCache; }

	float GetBeginNotifyTime() const { return m_beginNotifyTime; }

	uint32_t GetTagID() const { return m_tagID; }

	void SetComponentObserverCache(std::weak_ptr<ComponentObserver> Set) { m_componentObserverCache = Set; }

private:

	std::weak_ptr<ComponentObserver> m_componentObserverCache;

	uint32_t m_tagID = CommonConstant::INVALID_STATIC_ID;

	float m_beginNotifyTime = 0.0f;
};