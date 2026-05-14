#include "BTNodeBranchBase.h"

#include "../../../../Utility/ImGui/ImGuiUtility.h"

#include "../../../../../System/Input/RawInputManager.h"

#include "../../../../GUID/BehaviorTree/GUIDBehaviorTreeManager.h"

#include "../Root/BTNodeRoot.h"

void BTNodeBranchBase::Init()
{
	BTNodeBase::Init();

	m_nowConditionMatchedIndex = CommonEnum::BranchNodeType::False;
}
void BTNodeBranchBase::PostLoadInit()
{
	BTNodeBase::PostLoadInit();

	for (const auto& child_ : m_childNodeList)
	{
		if (!child_) { continue; }

		child_->PostLoadInit();
	}
}

void BTNodeBranchBase::Tick()
{
	// 条件に応じて実行するノードを切り替える
	m_nowConditionMatchedIndex = IsConditionMatched() ? CommonEnum::BranchNodeType::True : CommonEnum::BranchNodeType::False;

	// 容量を超えていないかを確認
	int index_ = static_cast<int>(m_nowConditionMatchedIndex);
	if (index_ >= static_cast<int>(CommonEnum::BranchNodeType::MaxSize)) { return; }

	auto childNode_ = m_childNodeList[index_];
	if (!childNode_) { return; }

	// 子ノードを実行しその結果を反映
	childNode_->Tick();
	SetNodeState(childNode_->GetNodeState());
}

void BTNodeBranchBase::OnDestroy()
{
	auto rootNode_ = GetNodeRootCache().lock();
	if (!rootNode_) { return; }

	BTNodeBase::OnDestroy();

	for (const auto& child_ : m_childNodeList)
	{
		if (!child_) { continue; }

		rootNode_->AddCandidateNodeList(child_);
	}
}

void BTNodeBranchBase::ImGuiNodeInspector()
{
	auto rootNode_ = GetNodeRootCache().lock();
	if (!rootNode_) { return; }

	const auto& inputManager_ = RawInputManager::GetInstance();

	const uint64_t rootGUID_ = GetGUID();

	ImNodes::PushColorStyle(ImNodesCol_TitleBar , ImGuiUtility::BRANCH_NODE_COLOR);
	BTNodeBase::ImGuiNodeInspector();
	ImNodes::PopColorStyle();

	// 子ノードのリンクの描画
	for (const auto& childNode_ : m_childNodeList)
	{
		if (!childNode_) { continue; }

		ImGuiUtility::DrawLink(rootGUID_ , childNode_->GetGUID());
		childNode_->ImGuiNodeInspector();
	}

	// ノードが選択されていて"Reset"キーが押されていたら該当ノードをリセット
	if (!m_childNodeList.empty())
	{
		auto itr_ = m_childNodeList.begin();
		while (itr_ != m_childNodeList.end())
		{
			auto childNode_ = *itr_;
			if (!childNode_) 
			{
				itr_++;
				continue; 
			}
		
			const int selfID_ = static_cast<int>(childNode_->GetGUID());
			
			// もし該当ノードが選択されていなければ実行しない
			if (ImNodes::IsNodeSelected(selfID_))
			{
				ImGui::PushID(selfID_);

				rootNode_->SetSelectedNode(childNode_);

				// もし右クリックされたらコンテキストメニューを表示
				if (inputManager_.IsMouseClickedOnce(VK_RBUTTON))
				{
					ImGui::OpenPopup("NodeContextMenu");
				}

				if (ImGui::BeginPopup("NodeContextMenu"))
				{
					// "ID"の登録を解除してから削除
					if (ImGui::MenuItem("Reset"))
					{
						childNode_->OnDestroy();
						*itr_ = nullptr;
						ImGui::EndPopup();
						ImGui::PopID();
						continue;
					}

					// 候補ノードに自信を格納
					if (ImGui::MenuItem("CancelNodeLink"))
					{
						rootNode_->AddCandidateNodeList(childNode_);
						*itr_ = nullptr;
						ImGui::EndPopup();
						ImGui::PopID();
						continue;
					}

					ImGui::EndPopup();
				}

				ImGui::PopID();
			}
			
			itr_++;
		}
	}
}
void BTNodeBranchBase::ImGuiAfterEndNodeEditorInspector()
{
	BTNodeBase::ImGuiAfterEndNodeEditorInspector();

	// もしリンクが生成されたノードのポインタを取得出来たら格納、
	// そして上から実行されるようにソート
	auto linked_ = ImGuiUtility::ImGuiReceiveLinkedNode(GetGUID(), GetNodeRootCache());
	if (linked_)
	{
		LinkChildNode(linked_);
	}

	// ノードが存在したらソート
	{
		const int trueNode_  = static_cast<int>(CommonEnum::BranchNodeType::True );
		const int falseNode_ = static_cast<int>(CommonEnum::BranchNodeType::False);

		if (m_childNodeList[trueNode_] && m_childNodeList[falseNode_])
		{
			std::sort(m_childNodeList.begin(), m_childNodeList.end(), [](const auto& Self, const auto& Pair)
			{
				return (Self->GetNodePos().y < Pair->GetNodePos().y);
			});
		}
	}

	// 子ノードもリンク接続を実行
	for (const auto& childNode_ : m_childNodeList)
	{
		if (!childNode_) { continue; }
		childNode_->ImGuiAfterEndNodeEditorInspector();
	}
}

void BTNodeBranchBase::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializeSpawnData(Json);

	// "True"ノードの要素を作成
	DeserializeBranchNodeSpawnData(Json , CommonEnum::BranchNodeType::True , "TrueNode");

	// "False"ノードの要素を作成
	DeserializeBranchNodeSpawnData(Json , CommonEnum::BranchNodeType::False , "FalseNode");
}
void BTNodeBranchBase::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializePrefabData(Json);

	// "True"ノードの要素を作成
	DeserializeBranchNodePrefabData(Json , CommonEnum::BranchNodeType::True , "TrueNodeName" , "TrueNode");

	// "False"ノードの要素を作成
	DeserializeBranchNodePrefabData(Json, CommonEnum::BranchNodeType::False , "FalseNodeName", "FalseNode");
}

nlohmann::json BTNodeBranchBase::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	// 基底クラスのシリアライズ
	{
		auto baseJson_ = BTNodeBase::SerializeSpawnData();
		if (!baseJson_.is_null())
		{
			json_.update(baseJson_);
		}
	}
	
	// "True"ノードのシリアライズ
	{
		auto trueNode_ = SerializeBranchNodeSpawnData(CommonEnum::BranchNodeType::True , "TrueNode");
		if (!trueNode_.is_null())
		{
			json_.update(trueNode_);
		}
	}

	// "False"ノードのシリアライズ
	{
		auto falseNode_ = SerializeBranchNodeSpawnData(CommonEnum::BranchNodeType::False , "FalseNode");
		if (!falseNode_.is_null())
		{
			json_.update(falseNode_);
		}
	}

	return json_;
}
nlohmann::json BTNodeBranchBase::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	// 基底クラスのシリアライズ
	{
		auto baseJson_ = BTNodeBase::SerializePrefabData();
		if (!baseJson_.is_null())
		{
			json_.update(baseJson_);
		}
	}

	// "True"ノードのシリアライズ
	{
		auto trueNode_ = SerializeBranchNodePrefabData(CommonEnum::BranchNodeType::True , "TrueNodeName" , "TrueNode");
		if (!trueNode_.is_null())
		{
			json_.update(trueNode_);
		}
	}

	// "False"ノードのシリアライズ
	{
		auto falseNode_ = SerializeBranchNodePrefabData(CommonEnum::BranchNodeType::False , "FalseNodeName" , "FalseNode");
		if (!falseNode_.is_null())
		{
			json_.update(falseNode_);
		}
	}

	return json_;
}

void BTNodeBranchBase::DeserializeBranchNodeSpawnData(const nlohmann::json& Json, CommonEnum::BranchNodeType NodeIndex, const char* NodeKey)
{
	int index_ = static_cast<int>(NodeIndex);

	if (Json.is_null())													 { return; }
	if (index_ >= static_cast<int>(CommonEnum::BranchNodeType::MaxSize)) { return; }

	// "True"ノードの要素を作成
	if (auto node_ = m_childNodeList[index_])
	{
		if (Json.contains(NodeKey))
		{
			node_->DeserializeSpawnData(Json[NodeKey]);
		}
	}
}
void BTNodeBranchBase::DeserializeBranchNodePrefabData(const nlohmann::json& Json, CommonEnum::BranchNodeType NodeIndex, const char* NodeNameKey, const char* NodeKey)
{
	int index_ = static_cast<int>(NodeIndex);

	if (Json.is_null())																		  { return; }
	if (static_cast<int>(NodeIndex) >= static_cast<int>(CommonEnum::BranchNodeType::MaxSize)) { return; }

	if (!m_childNodeList[index_])
	{
		const std::string nodeName_ = Json.value(NodeNameKey , CommonConstant::STRING_UNKNOWN);
		auto created_ = BTNodeFactory::GetInstance().Create(nodeName_);
		if (created_) 
		{
			created_->Init				();
			created_->SetBlackBoardCache(GetBlackBoardCache());
			created_->SetNodeRootCache	(GetNodeRootCache());

			if (Json.contains(NodeKey))
			{
				created_->DeserializePrefabData(Json[NodeKey]);
			}

			m_childNodeList[index_] = created_;
		}
	}
}

nlohmann::json BTNodeBranchBase::SerializeBranchNodeSpawnData(CommonEnum::BranchNodeType NodeIndex, const char* NodeKey)
{
	auto json_ = nlohmann::json();

	int index_ = static_cast<int>(NodeIndex);

	if (index_ >= static_cast<int>(CommonEnum::BranchNodeType::MaxSize))
	{
		return json_; 
	}

	if (auto node_ = m_childNodeList[index_])
	{
		auto nodeJson_ = node_->SerializeSpawnData();
		if (!nodeJson_.is_null())
		{
			json_[NodeKey] = nodeJson_;
		}
	}

	return json_;
}
nlohmann::json BTNodeBranchBase::SerializeBranchNodePrefabData(CommonEnum::BranchNodeType NodeIndex, const char* NodeNameKey, const char* NodeKey)
{

	auto json_ = nlohmann::json();

	int index_ = static_cast<int>(NodeIndex);

	if (index_ >= static_cast<int>(CommonEnum::BranchNodeType::MaxSize))
	{
		return json_;
	}

	if (auto node_ = m_childNodeList[index_])
	{
		json_[NodeNameKey] = node_->GetNodeName();

		auto nodeJson_ = node_->SerializePrefabData();
		if (!nodeJson_.is_null())
		{
			json_[NodeKey] = nodeJson_;
		}
	}

	return json_;
}

void BTNodeBranchBase::ChangeChildNodeType(const int Index, std::shared_ptr<BTNodeBase> Child)
{
	if (!Child) { return; }

}
void BTNodeBranchBase::LinkChildNode(std::shared_ptr<BTNodeBase> Child)
{
	if (!Child) { return; }

	// ヌルポインタのノードに格納
	for (auto& node_ : m_childNodeList)
	{
		if (node_) { continue; }
		
		node_ = Child;
		break;
	}
}