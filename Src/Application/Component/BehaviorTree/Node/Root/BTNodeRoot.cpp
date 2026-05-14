#include "BTNodeRoot.h"

#include "../Composite/BTNodeCompositeBase.h"

#include "../../../../Utility/ImGui/ImGuiUtility.h"
#include "../../../../Utility/BehaviorTree/BTUtility.h"

#include "../../../../../System/Input/RawInputManager.h"

void BTNodeRoot::PostLoadInit()
{
	BTNodeBase::PostLoadInit();

	if (m_childNode)
	{
		m_childNode->PostLoadInit ();
	}
}
void BTNodeRoot::Tick()
{
	if (m_childNode)
	{
		m_childNode->Tick();
	}

}

void BTNodeRoot::ImGuiPrefabDataInspector()
{
	// コンポジットノードの選択
	if (ImGui::CollapsingHeader("RootNodePrefabData"))
	{
		{
			ImGuiUtility::BTNodeSelectorConditional<BTNodeCompositeBase>("RootNode Child Selector" , m_childNode, GetBlackBoardCache() , CastSelf());
		}

		// エディター上に追加するためだけのリスト
		{
			std::shared_ptr<BTNodeBase> addCandidateNode_ = nullptr;
			ImGuiUtility::BTNodeSelector("Add CandidateNode" , addCandidateNode_ , GetBlackBoardCache() , CastSelf());
			if (addCandidateNode_)
			{
				AddCandidateNodeList(addCandidateNode_);
			}
		}
	}

	if (auto selectedNode_ = m_selectedNode.lock())
	{
		if (ImGui::CollapsingHeader("SelectedNodePrefabData"))
		{
			selectedNode_->ImGuiPrefabDataInspector();
		}
	}
}
void BTNodeRoot::ImGuiNodeInspector()
{
	ImNodes::BeginNodeEditor();

	ImGuiRootNodeInspector     ();
	ImGuiCandidateNodeInspector();

	const uint64_t rootGUID_ = GetGUID();

	// 子ノードの描画(コンポジットノード)
	if (m_childNode)
	{
		ImGuiUtility::DrawLink(rootGUID_ , m_childNode->GetGUID());
		m_childNode->ImGuiNodeInspector();
	}

	// ミニマップの描画
	ImNodes::MiniMap(ImGuiUtility::NODE_EDITOR_MINIMAP_SIZE , ImNodesMiniMapLocation_TopRight);
	ImNodes::EndNodeEditor();
}
void BTNodeRoot::ImGuiAfterEndNodeEditorInspector()
{
	BTNodeBase::ImGuiAfterEndNodeEditorInspector();

	if (m_childNode)
	{
		m_childNode->ImGuiAfterEndNodeEditorInspector();
	}

	// もしリンクされたならリストから除外する
	{
		auto itr_ = m_candidateNodeList.begin();

		while (itr_ != m_candidateNodeList.end())
		{
			if (itr_->isLinked)
			{
				itr_ = m_candidateNodeList.erase(itr_);
			}
			else
			{
				itr_++;
			}
		}
	}
}

void BTNodeRoot::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializeSpawnData(Json);

	if (m_childNode && Json.contains("ChildNode"))
	{
		m_childNode->DeserializeSpawnData(Json["ChildNode"]);
	}
}
void BTNodeRoot::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializePrefabData(Json);

	// ノード名からノードを作成
	const std::string nodeName_ = Json.value("ChildNodeName", CommonConstant::STRING_UNKNOWN);
	auto created_ = BTNodeFactory::GetInstance().Create(nodeName_);

	// もし作成できなければ格納しない
	if (created_)
	{
		created_->Init				();
		created_->SetBlackBoardCache(GetBlackBoardCache());
		created_->SetNodeRootCache  (CastSelf());
	
		if (Json.contains("ChildNode"))
		{
			created_->DeserializePrefabData(Json["ChildNode"]);
		}

		// デシリアライズしたものを格納
		m_childNode = created_;
	}
}

nlohmann::json BTNodeRoot::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	// 基底クラスの"Json"
	{
		auto baseJson_ = BTNodeBase::SerializeSpawnData();

		if (!baseJson_.is_null())
		{
			json_.update(baseJson_);
		}
	}

	if (m_childNode)
	{
		json_["ChildNode"] = m_childNode->SerializeSpawnData();
	}

	return json_;
}
nlohmann::json BTNodeRoot::SerializePrefabData()
{
	auto json_ = nlohmann::json();
	
	// 基底クラスの"Json"
	{
		auto baseJson_ = BTNodeBase::SerializePrefabData();

		if (!baseJson_.is_null())
		{
			json_.update(baseJson_);
		}
	}

	if (m_childNode)
	{
		json_["ChildNodeName"] = m_childNode->GetNodeName        ();
		json_["ChildNode"    ] = m_childNode->SerializePrefabData();
	}

	return json_;
}

void BTNodeRoot::AddCandidateNodeList(std::shared_ptr<BTNodeBase> Node)
{
	if (!Node) { return; }
	m_candidateNodeList.emplace_back(Node);
}

void BTNodeRoot::ImGuiRootNodeInspector()
{
	const uint64_t rootGUID_ = GetGUID();

	// ルートノード自身の描画
	// ノードの色変更
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImGuiUtility::ROOT_NODE_COLOR);

	ImNodes::BeginNode(static_cast<int>(rootGUID_));
	{
		ImGuiUtility::DrawNodeTitleBar(GetNodeName());

		ImGui::Text("GUID : %llu", GetGUID());

		// ノード状態の描画
		for (const auto& state_ : ImGuiUtility::NODE_STATE_LIST)
		{
			if (GetNodeState() != state_.type) { continue; }
			ImGui::Text("State : %s", state_.label);
		}

		ImGuiUtility::DrawNodeOutputAttribute(rootGUID_);
	}
	ImNodes::EndNode();

	ImNodes::PopColorStyle();

	// ノード座標を毎フレーム格納
	SetNodePos(ImNodes::GetNodeEditorSpacePos(static_cast<int>(GetGUID())));
}
void BTNodeRoot::ImGuiCandidateNodeInspector()
{
	const auto& inputManager_ = RawInputManager::GetInstance();

	// 追加候補ノードを描画
	{
		for (const auto& candidateNode_ : m_candidateNodeList)
		{
			if (!candidateNode_.node) { continue; }

			candidateNode_.node->ImGuiNodeInspector();
		}
	}

	// 追加候補ノードの削除
	{
		auto itr_ = m_candidateNodeList.begin();
		while (itr_ != m_candidateNodeList.end())
		{
			auto childNode_ = itr_->node;

			if (!childNode_)
			{
				itr_ = m_candidateNodeList.erase(itr_);
				continue;
			}

			const int selfID_ = static_cast<int>(childNode_->GetGUID());

			// もし該当ノードが選択されていなければ実行しない
			if (ImNodes::IsNodeSelected(selfID_))
			{
				ImGui::PushID(selfID_);

				SetSelectedNode(childNode_);

				// もし右クリックされたらコンテキストメニューを表示
				if (inputManager_.IsMouseClickedOnce(VK_RBUTTON))
				{
					ImGui::OpenPopup("NodeContextMenu");
				}

				if (ImGui::BeginPopup("NodeContextMenu"))
				{
					// "ID"の登録を解除してから削除
					if (ImGui::MenuItem("Delete"))
					{
						childNode_->OnDestroy();
						itr_ = m_candidateNodeList.erase(itr_);
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

std::shared_ptr<BTNodeRoot> BTNodeRoot::CastSelf()
{
	const uint32_t typeID_ = shared_from_this()->GetTypeID();
	if (typeID_ != StaticID<BTNodeBase>::GetTypeID<BTNodeRoot>()) 
	{
		return std::shared_ptr<BTNodeRoot>();
	}

	return std::static_pointer_cast<BTNodeRoot>(shared_from_this());
}