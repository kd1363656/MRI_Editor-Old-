#pragma once

template <std::size_t Size>
class EventChannel
{
public:

	EventChannel () = default;
	~EventChannel() = default;

	void BeginFrame()
	{
		m_momentEvent.fill(false);
	}

	void ClearAllNotify()
	{
		m_momentEvent.fill(false);
		m_keepEvent.fill  (false);
	}

	void ClearLaneAllNotify(const CommonEnum::EventLane Lane)
	{
		if (Lane == CommonEnum::EventLane::Moment)
		{
			m_momentEvent.fill(false);
		}
		else
		{
			m_keepEvent.fill(false);
		}
	}

	// 特定のレーンの特定のイベントに通知を送る関数
	void NotifyEvent(const std::size_t Index , const CommonEnum::EventLane Lane , bool Notify)
	{
		if (!IsInCapacity(Index)) { return; }

		if(Lane == CommonEnum::EventLane::Moment) 
		{
			m_momentEvent[Index] = Notify;
		}
		else
		{
			m_keepEvent[Index] = Notify;
		}
	}

	bool HasMomentNotify(const std::size_t Index)
	{
		if (!IsInCapacity(Index)) { return false; }

		return m_momentEvent[Index];
	}

	bool HasKeepNotify(const std::size_t Index)
	{
		if (!IsInCapacity(Index)) { return false; }

		return m_keepEvent[Index];
	}

	static constexpr bool IsInCapacity(const std::size_t Index) 
	{
		// 要素を超えていたら"false"
		return Index < Size; 
	}

private:

	std::array<bool , Size> m_momentEvent;
	std::array<bool , Size> m_keepEvent;

};