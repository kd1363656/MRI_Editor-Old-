#include "InputPlayerComponent.h"

#include "../../../GameObject/GameObject.h"

#include "../../../Component/Move/Input/BasedOnTarget/MoveInputBasedOnTargetComponent.h"
#include "../../../Component/Rotation/Input/BasedOnTarget/Smooth/RotationInputBasedOnTargetSmoothComponent.h"
#include "../../../Component/Rotation/Input/BasedOnTarget/Instant/RotationInputBasedOnTargetInstantComponent.h"
#include "../../../CommonCore/Move/Input/CommonCoreMoveInput.h"
#include "../../../CommonCore/Rotation/CommonCoreRotation.h"
#include "../../../CommonCore/Rotation/Input/Smooth/CommonCoreRotationInputSmooth.h"
#include "../../../Component/Render/Animatoin/Model/RenderAnimationModelComponent.h"

#include "../../../Observer/Component/ComponentObserver.h"
#include "../../../Tag/ComponentEvent/ComponentEventTag.h"

#include "../../../Utility/Input/InputUtility.h"

#include "../../../main.h"

#include "../../../../System/Calculation/Calculation.h"

void InputPlayerComponent::Init()
{
	m_receiveInputAtk = false;
}
void InputPlayerComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	m_moveInputBasedOnTargetComponentCache            = owner_->GetComponent<MoveInputBasedOnTargetComponent>           ();
	m_rotationInputBasedOnTargetSmoothComponentCache  = owner_->GetComponent<RotationInputBasedOnTargetSmoothComponent> ();
	m_rotationInputBasedOnTargetInstantComponentCache = owner_->GetComponent<RotationInputBasedOnTargetInstantComponent>();
	m_renderAnimationModelComponentCache              = owner_->GetComponent<RenderAnimationModelComponent>	           ();
}

void InputPlayerComponent::EarlyUpdate()
{
	MoveInput    ();
	RotationInput();
	AtkInput     ();
}

void InputPlayerComponent::ImGuiPrefabDataInspector()
{
	ImGui::Checkbox("ReceiveInputAtk" , &m_receiveInputAtk);
}

void InputPlayerComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_receiveInputAtk = Json.value("ReceiveInputAtk" , false);
}

nlohmann::json InputPlayerComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["ReceiveInputAtk"] = m_receiveInputAtk;

	return json_;
}

void InputPlayerComponent::MoveInput()
{
	auto& globalObserver_ = Application::Instance().GetGlobalObserver();

	if (auto moveInputBaseOnTarget_ = m_moveInputBasedOnTargetComponentCache.lock())
	{
		// 移動入力の確認
		if (auto commonCoreMoveInput_ = moveInputBaseOnTarget_->GetCommonCoreMoveInputCache().lock())
		{
			// 移動方角をクリア
			commonCoreMoveInput_->ResetInputMoveDirection();
			Move(globalObserver_ , commonCoreMoveInput_);
		}
	}
}

void InputPlayerComponent::RotationInput()
{
	auto& globalObserver_ = Application::Instance().GetGlobalObserver();

	// 回転入力の確認
	// 徐々に回転するコンポーネント
	if (auto rotationInputBasedOnTargetSmooth_ = m_rotationInputBasedOnTargetSmoothComponentCache.lock())
	{
		if (auto commonCoreRotationInputSmoothCache_ = rotationInputBasedOnTargetSmooth_->GetCommonCoreRotationInputSmoothCache().lock())
		{
			if (auto commonCoreRotationCache_ = commonCoreRotationInputSmoothCache_->GetCommonCoreRotationCache().lock())
			{
				// 回転方向をクリア
				commonCoreRotationCache_->ResetRotationDirection();
				Rotation(globalObserver_, commonCoreRotationCache_);
			}
		}
	}

	// 瞬時に回転するコンポーネント
	// 攻撃時にのみ発動
	if (auto rotationInputBasedOnTargetInstant_ = m_rotationInputBasedOnTargetInstantComponentCache.lock())
	{
		if (auto componentCommonCoreRotationCache_ = rotationInputBasedOnTargetInstant_->GetComponentCommonCoreRotationCache().lock())
		{
			// 回転方向をクリア
			componentCommonCoreRotationCache_->ResetRotationDirection();
			Rotation(globalObserver_, componentCommonCoreRotationCache_);
		}
	}
}

void InputPlayerComponent::AtkInput()
{
	// 入力を受け取らなければ"return"
	if (!m_receiveInputAtk) { return; }

	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto observer_ = owner_->GetComponentObserver().lock();
	if (!observer_) { return; }

	auto& globalObserver_ = Application::Instance().GetGlobalObserver();

	// 攻撃の入力を確認
	if (globalObserver_.HasMouseMomentNotify(GlobalEvent::MouseEvent::ClickLeft))
	{
		// 該当する攻撃ステート全てを通知
		observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<AtkComboFirstActiveTag> (), CommonEnum::EventLane::Moment, CommonEnum::BoolFlag::True);
		observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<AtkComboSecondActiveTag>(), CommonEnum::EventLane::Moment, CommonEnum::BoolFlag::True);
		observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<AtkComboThirdActiveTag> (), CommonEnum::EventLane::Moment, CommonEnum::BoolFlag::True);
		observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<AtkComboFourthActiveTag>(), CommonEnum::EventLane::Moment, CommonEnum::BoolFlag::True);
	}
}

void InputPlayerComponent::Move(GlobalObserver& Observer, std::shared_ptr<CommonCoreMoveInput> CommonCoreMove)
{
	if (!CommonCoreMove) { return; }

	auto owner_ = GetOwner();
	if (!owner_) { return; }

	auto observer_ = owner_->GetComponentObserver().lock();
	if (!observer_) { return; }

	const bool pressLeft_  = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressLeft);
	const bool pressRight_ = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressRight);

	const bool pressUp_   = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressUp);
	const bool pressDown_ = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressDown);

	if (!pressRight_ && !pressLeft_ && !pressUp_ && !pressDown_) 
	{
		observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<MoveInputBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep, CommonEnum::BoolFlag::False);
		return;
	}

	// 反対方向のキーが押されていたら移動しない
	if (!pressLeft_ || !pressRight_)
	{
		if (pressLeft_)
		{
			Calculation::AddDirection(Math::Vector3::Left, CommonCoreMove->GetWorkInputMoveDirection());
			observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<MoveInputBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep, CommonEnum::BoolFlag::True);
		}
		else if (pressRight_)
		{
			Calculation::AddDirection(Math::Vector3::Right, CommonCoreMove->GetWorkInputMoveDirection());
			observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<MoveInputBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep, CommonEnum::BoolFlag::True);
		}
	}

	// 反対方向のキーが押されていたら移動しない
	if (!pressUp_ || !pressDown_)
	{
		if (pressUp_)
		{
			Calculation::AddDirection(Math::Vector3::Backward, CommonCoreMove->GetWorkInputMoveDirection());
			observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<MoveInputBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep, CommonEnum::BoolFlag::True);
		}
		else if (pressDown_)
		{
			Calculation::AddDirection(Math::Vector3::Forward, CommonCoreMove->GetWorkInputMoveDirection());
			observer_->NotifyEvent(StaticID<ComponentEventTagBase>::GetTypeID<MoveInputBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep, CommonEnum::BoolFlag::True);
		}
	}
}

void InputPlayerComponent::Rotation(GlobalObserver& Observer, std::shared_ptr<CommonCoreRotation> CommonCoreRotation)
{
	if (!CommonCoreRotation) { return; }

	const bool pressLeft_  = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressLeft );
	const bool pressRight_ = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressRight);
	const bool pressUp_    = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressUp   );
	const bool pressDown_  = Observer.HasKeyKeepNotify(GlobalEvent::KeyEvent::PressDown );

	// 反対方向のキーが押されていたら移動しない
	if (!pressLeft_ || !pressRight_)
	{
		if (pressLeft_)
		{
			Calculation::AddDirection(Math::Vector3::Left , CommonCoreRotation->GetWorkRotationDirection());
		}
		else if (pressRight_)
		{
			Calculation::AddDirection(Math::Vector3::Right , CommonCoreRotation->GetWorkRotationDirection());
		}
	}

	// 反対方向のキーが押されていたら移動しない
	if (!pressUp_ || !pressDown_)
	{
		if (pressUp_)
		{
			Calculation::AddDirection(Math::Vector3::Backward, CommonCoreRotation->GetWorkRotationDirection());
		}
		else if (pressDown_)
		{
			Calculation::AddDirection(Math::Vector3::Forward, CommonCoreRotation->GetWorkRotationDirection());
		}
	}
}