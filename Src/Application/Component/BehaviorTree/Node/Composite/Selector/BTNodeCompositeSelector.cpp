#include "BTNodeCompositeSelector.h"

void BTNodeCompositeSelector::Tick()
{
	// 全ノードを捜査して"Success"状態が一つでもあれば自身の状態を"Success"にする
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
		else if (nodeState_ == BTNodeBase::NodeState::Success)
		{
			SetNodeState(BTNodeBase::NodeState::Success);
			return;
		}
	}

	// もし全ノードから"Success"が一つもなければ自身の状態を"Fail"とする
	SetNodeState(BTNodeBase::NodeState::Fail);
}