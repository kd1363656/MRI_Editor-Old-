#include "BTNodeCompositeSequence.h"

void BTNodeCompositeSequence::Tick()
{
	// もし全ノードの状態が"Success"だったら自身の状態を"Success"にする
	for (auto& child_ : GetChildNodeList())
	{
		if (!child_) { continue; }

		// 子ノードがもし待機状態なら初期化する
		if (child_->GetNodeState() == BTNodeBase::NodeState::Idle)
		{
			child_->Init();
		}

		child_->Tick();

		// 更新処理をした後のノードの状態を取得
		BTNodeBase::NodeState nodeState_ = child_->GetNodeState();
		if (nodeState_ == BTNodeBase::NodeState::Running)
		{
			// 子の状態が"Running"なら自身も"Running"で"return"
			SetNodeState(BTNodeBase::NodeState::Running);
			return;
		}
		else if (nodeState_ == BTNodeBase::NodeState::Fail)
		{
			SetNodeState(BTNodeBase::NodeState::Fail);
			return;
		}
	}

	SetNodeState(BTNodeBase::NodeState::Success);
}