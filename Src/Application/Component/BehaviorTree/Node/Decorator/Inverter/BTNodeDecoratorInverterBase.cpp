#include "BTNodeDecoratorInverterBase.h"

#include "../../Root/BTNodeRoot.h"

void BTNodeDecoratorInverterBase::Tick()
{
	const BTNodeBase::NodeState childNodeState_ = GetChildState();

	// 条件と一致したら子ノードの処理を実行
	if (IsConditionMatched())
	{
		TickChild();
	}

	if (childNodeState_ == BTNodeBase::NodeState::Success)
	{
		SetNodeState(BTNodeBase::NodeState::Fail);
		return;
	}
	else if (childNodeState_ == BTNodeBase::NodeState::Fail)
	{
		SetNodeState(BTNodeBase::NodeState::Success);
		return;
	}

	// 子ノードが実行中ならこのノードも実行中にする
	SetNodeState(BTNodeBase::NodeState::Running);
}