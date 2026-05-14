#pragma once
#include "../../../Application/Component/BehaviorTree/Node/BTNodeBase.h"

namespace BTUtility
{
	template <typename NodeType>
		requires std::derived_from<NodeType , BTNodeBase>
	static std::weak_ptr<NodeType> Cast(std::weak_ptr<BTNodeBase> Node)
	{
		auto node_ = Node.lock();
		if (!node_) 
		{ 
			return std::weak_ptr<NodeType>(); 
		}

		// 自身の"ID"
		const uint32_t id_ = node_->GetTypeID();

		// 型"ID"と自身の保有する"ID"に整合性がないなら"return"
		if (id_ != StaticID<BTNodeBase>::GetTypeID<NodeType>()) 
		{
			return std::weak_ptr<NodeType>(); 
		}

		return std::static_pointer_cast<NodeType>(node_);
	}
}