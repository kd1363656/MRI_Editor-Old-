#include "GlobalObserver.h"

#include "../../main.h"

#include "../../Utility/Input/InputUtility.h"

#include "EventChannel/EventChannel.h"

void GlobalObserver::Init()
{
	if (!m_keyEvent)
	{
		m_keyEvent = std::make_shared<EventChannel<KEY_MAX_COUNT>>();
	}

	if (!m_mouseEvent)
	{
		m_mouseEvent = std::make_shared<EventChannel<MOUSE_MAX_COUNT>>();
	}

	m_keyEvent->ClearAllNotify  ();
	m_mouseEvent->ClearAllNotify();
}

void GlobalObserver::BeginFrame()
{
	// 一瞬だけのイベントはここでクリア
	m_keyEvent->BeginFrame  ();
	m_mouseEvent->BeginFrame();
}

void GlobalObserver::UpdateKeyEvent()
{
	if (!m_keyEvent) { return; }

	// 入力状況を常に確認
	// キー
	// デバック状況は反応しない
#ifdef _DEBUG
	//if (!KdDebugGUI::Instance().GetCanShowDebugWindowFlag())
#endif
	{
		NotifyKeyEvent(CommonEnum::UseKeyInputFlags::KeyLeft , GlobalEvent::KeyEvent::PressLeft , CommonEnum::EventLane::Keep);
		NotifyKeyEvent(CommonEnum::UseKeyInputFlags::KeyRight, GlobalEvent::KeyEvent::PressRight, CommonEnum::EventLane::Keep);
		NotifyKeyEvent(CommonEnum::UseKeyInputFlags::KeyDown , GlobalEvent::KeyEvent::PressDown , CommonEnum::EventLane::Keep);
		NotifyKeyEvent(CommonEnum::UseKeyInputFlags::KeyUp   , GlobalEvent::KeyEvent::PressUp   , CommonEnum::EventLane::Keep);

		NotifyMouseEvent(CommonEnum::UseMouseInputFlags::ClickMouseLeft, GlobalEvent::MouseEvent::ClickLeft, CommonEnum::EventLane::Moment);
	}
}

void GlobalObserver::NotifyKeyEvent(GlobalEvent::KeyEvent Event, CommonEnum::EventLane Lane, bool Notify)
{
	if (!m_keyEvent) { return; }

	m_keyEvent->NotifyEvent(ToValueUtility::EnumToValue<size_t>(Event), Lane, Notify);
}
void GlobalObserver::NotifyMouseEvent(GlobalEvent::MouseEvent Event, CommonEnum::EventLane Lane, bool Notify)
{
	if (!m_mouseEvent) { return; }

	m_mouseEvent->NotifyEvent(ToValueUtility::EnumToValue<size_t>(Event) , Lane , Notify);
}

bool GlobalObserver::HasKeyMomentNotify(GlobalEvent::KeyEvent Event)
{
	if (!m_keyEvent) { return false; }

	return m_keyEvent->HasMomentNotify(ToValueUtility::EnumToValue<size_t>(Event));
}
bool GlobalObserver::HasKeyKeepNotify(GlobalEvent::KeyEvent Event)
{
	if (!m_keyEvent) { return false; }

	return m_keyEvent->HasKeepNotify(ToValueUtility::EnumToValue<size_t>(Event));
}

bool GlobalObserver::HasMouseMomentNotify(GlobalEvent::MouseEvent Event)
{
	if (!m_mouseEvent) { return false; }

	return m_mouseEvent->HasMomentNotify(ToValueUtility::EnumToValue<size_t>(Event));
}
bool GlobalObserver::HasMouseKeepNotify(GlobalEvent::MouseEvent Event)
{
	if (!m_mouseEvent) { return false; }

	return m_mouseEvent->HasKeepNotify(ToValueUtility::EnumToValue<size_t>(Event));
}

void GlobalObserver::NotifyKeyEvent(CommonEnum::UseKeyInputFlags CheckKey, GlobalEvent::KeyEvent Event, CommonEnum::EventLane Lane)
{
	// 入力がなければ通知をやめる
	const bool isKeyHeld_ = InputUtility::IsKeyHeld(CheckKey);
	
	NotifyKeyEvent(Event , Lane , isKeyHeld_);
}

void GlobalObserver::NotifyMouseEvent(CommonEnum::UseMouseInputFlags CheckMouse, GlobalEvent::MouseEvent Event, CommonEnum::EventLane Lane)
{
	const bool isMouseClicked_ = InputUtility::IsMouseJustClicked(CheckMouse);

	NotifyMouseEvent(Event , Lane , isMouseClicked_);
}