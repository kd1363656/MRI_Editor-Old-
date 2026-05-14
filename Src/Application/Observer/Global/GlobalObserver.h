#pragma once
#include "Event/GlobalObserverEvent.h"
#include "../../Utility/Singleton/SingletonBase.h"
#include "../../Utility/ToValue/ToValueUtility.h"
#include "EventChannel/EventChannel.h"

template <std::size_t Size>
class EventChannel;

// 入力やゲームシーンの情報を提供する
class GlobalObserver
{

public:

	GlobalObserver () = default;
	~GlobalObserver() = default;

	void Init();

	void BeginFrame    ();
	void UpdateKeyEvent();

	void NotifyKeyEvent  (GlobalEvent::KeyEvent   Event , CommonEnum::EventLane Lane  , bool Notify);
	void NotifyMouseEvent(GlobalEvent::MouseEvent Event , CommonEnum::EventLane Lane  , bool Notify);

	bool HasKeyMomentNotify(GlobalEvent::KeyEvent Event);
	bool HasKeyKeepNotify  (GlobalEvent::KeyEvent Event);

	bool HasMouseMomentNotify(GlobalEvent::MouseEvent Event);
	bool HasMouseKeepNotify  (GlobalEvent::MouseEvent Event);
	
private:
	
	void NotifyKeyEvent  (CommonEnum::UseKeyInputFlags   CheckKey   , GlobalEvent::KeyEvent   Event , CommonEnum::EventLane Lane);
	void NotifyMouseEvent(CommonEnum::UseMouseInputFlags CheckMouse , GlobalEvent::MouseEvent Event , CommonEnum::EventLane Lane);

	static constexpr std::size_t KEY_MAX_COUNT   = ToValueUtility::EnumToValue<size_t>(GlobalEvent::KeyEvent::Max);
	static constexpr std::size_t MOUSE_MAX_COUNT = ToValueUtility::EnumToValue<size_t>(GlobalEvent::MouseEvent::Max);
	
	std::shared_ptr<EventChannel<KEY_MAX_COUNT  >> m_keyEvent;
	std::shared_ptr<EventChannel<MOUSE_MAX_COUNT>> m_mouseEvent;
};