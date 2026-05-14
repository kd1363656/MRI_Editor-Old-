#include "BTNodeDecoratorBase.h"

#include "../Root/BTNodeRoot.h"

void BTNodeDecoratorBase::Init()
{
	BTNodeBase::Init();
}
void BTNodeDecoratorBase::PostLoadInit()
{
	BTNodeBase::PostLoadInit();

	if (m_childNode)
	{
		m_childNode->PostLoadInit ();
	}
}

void BTNodeDecoratorBase::OnDestroy()
{
	auto rootNode_ = GetNodeRootCache().lock();
	if (!rootNode_) { return; }

	BTNodeBase::OnDestroy();

	// 子ノードのみルートノードに所有権を移す
	if (m_childNode)
	{
		rootNode_->AddCandidateNodeList(m_childNode);
	}
}

void BTNodeDecoratorBase::ImGuiNodeInspector()
{
	ImNodes::PushColorStyle(ImNodesCol_TitleBar , ImGuiUtility::DECORATOR_NODE_COLOR);
	BTNodeBase::ImGuiNodeInspector();
	ImNodes::PopColorStyle();

	const uint64_t selfGUID_ = GetGUID();

	if (m_childNode)
	{
		ImGuiUtility::DrawLink(selfGUID_ , m_childNode->GetGUID());
		m_childNode->ImGuiNodeInspector();
	}
}
void BTNodeDecoratorBase::ImGuiAfterEndNodeEditorInspector()
{
	BTNodeBase::ImGuiAfterEndNodeEditorInspector();

	// もしリンクが生成
	auto linked_ = ImGuiUtility::ImGuiReceiveLinkedNode(GetGUID() , GetNodeRootCache());
	if (linked_)
	{
		m_childNode = linked_;
	}

	if (m_childNode)
	{
		m_childNode->ImGuiAfterEndNodeEditorInspector();
	}
}

void BTNodeDecoratorBase::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializeSpawnData(Json);

	if (m_childNode)
	{
		if (Json.contains("ChildNode"))
		{
			m_childNode->DeserializeSpawnData(Json["ChildNode"]);
		}
	}
}
void BTNodeDecoratorBase::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializePrefabData(Json);

	if (!m_childNode)
	{
		// ファクトリーから生成したノードが存在したらデシリアライズ
		const std::string nodeName_ = Json.value("ChildNodeName", CommonConstant::STRING_UNKNOWN);
		auto created_ = BTNodeFactory::GetInstance().Create(nodeName_);
		if (created_)
		{
			created_->Init();
			created_->SetBlackBoardCache(GetBlackBoardCache());
			created_->SetNodeRootCache(GetNodeRootCache());

			if (Json.contains("ChildNode"))
			{
				created_->DeserializePrefabData(Json["ChildNode"]);
			}

			m_childNode = created_;
		}
	}
}

nlohmann::json BTNodeDecoratorBase::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	auto baseJson_ = BTNodeBase::SerializeSpawnData();
	if (!baseJson_.is_null())
	{
		json_.update(baseJson_);
	}

	if (m_childNode)
	{
		json_["ChildNode"] = m_childNode->SerializeSpawnData();
	}

	return json_;
}
nlohmann::json BTNodeDecoratorBase::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	auto baseJson_ = BTNodeBase::SerializePrefabData();
	if (!baseJson_.is_null())
	{
		json_.update(baseJson_);
	}

	if (m_childNode)
	{
		json_["ChildNodeName"] = m_childNode->GetNodeName		 ();
		json_["ChildNode"    ] = m_childNode->SerializePrefabData();
	}

	return json_;
}

void BTNodeDecoratorBase::TickChild()
{
	if (!m_childNode) { return; }

	// ノードが待機状態なら初期化
	if (GetChildState() == BTNodeBase::NodeState::Idle)
	{
		m_childNode->Init();
	}

	m_childNode->Tick();
}

BTNodeBase::NodeState BTNodeDecoratorBase::GetChildState()
{
	if (m_childNode)
	{
		return m_childNode->GetNodeState();
	}

	return BTNodeBase::NodeState::Fail;
}