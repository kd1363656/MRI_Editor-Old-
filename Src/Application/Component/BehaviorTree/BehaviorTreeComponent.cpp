#include "BehaviorTreeComponent.h"

#include "BlackBoard/BTBlackBoard.h"

#include "Node/Root/BTNodeRoot.h"

#include "../../Utility/String/StringUtility.h""

void BehaviorTreeComponent::Init()
{
	if (!m_blackBoard)
	{
		m_blackBoard = std::make_shared<BTBlackBoard>();
	}

	if (!m_nodeRoot)
	{
		m_nodeRoot = std::make_shared<BTNodeRoot>();
	}

	m_nodeRoot->Init			  ();
	m_nodeRoot->SetNodeName	      (StringUtility::StripClassPrefix<BTNodeRoot>());
	m_nodeRoot->SetBlackBoardCache(m_blackBoard);
}

void BehaviorTreeComponent::PostLoadInit()
{
	if (m_blackBoard)
	{
		m_blackBoard->SetOwnerCache(GetOwner());
		m_blackBoard->PostLoadInit ();
	}

	if (m_nodeRoot)
	{
		m_nodeRoot->PostLoadInit();
	}
}

void BehaviorTreeComponent::ImGuiSpawnDataInspector()
{
	if (m_blackBoard)
	{
		m_blackBoard->ImGuiSpawnDataInspector();
	}
}
void BehaviorTreeComponent::ImGuiPrefabDataInspector()
{
	if (m_blackBoard)
	{
		m_blackBoard->ImGuiPrefabDataInspector();
	}

	// ノードエディターの描画開始
	if (m_nodeRoot)
	{
		m_nodeRoot->ImGuiPrefabDataInspector();

		// エディター描画
		m_nodeRoot->ImGuiNodeInspector();
		
		// エディター描画が終わってから行う処理
		m_nodeRoot->ImGuiAfterEndNodeEditorInspector();
	}
}

void BehaviorTreeComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_blackBoard)
	{
		if (Json.contains("BlackBoard"))
		{
			m_blackBoard->DeserializeSpawnData(Json["BlackBoard"]);
		}
	}

	if (m_nodeRoot)
	{
		if (Json.contains("NodeRoot"))
		{
			m_nodeRoot->DeserializeSpawnData(Json["NodeRoot"]);
		}
	}
}

void BehaviorTreeComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }
	
	if (m_blackBoard)
	{
		if (Json.contains("BlackBoard"))
		{
			m_blackBoard->DeserializePrefabData(Json["BlackBoard"]);
		}
	}

	if (m_nodeRoot)
	{
		if (Json.contains("NodeRoot"))
		{
			m_nodeRoot->DeserializePrefabData(Json["NodeRoot"]);
		}
	}
}

nlohmann::json BehaviorTreeComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	if (m_blackBoard)
	{
		auto blackBoardJson_ = m_blackBoard->SerializeSpawnData();

		if (!blackBoardJson_.is_null())
		{
			json_["BlackBoard"] = blackBoardJson_;
		}
	}

	if (m_nodeRoot)
	{
		auto nodeRootJson_ = m_nodeRoot->SerializeSpawnData();

		if (!nodeRootJson_.is_null())
		{
			json_["NodeRoot"] = nodeRootJson_;
		}
	}

	return json_;
}

nlohmann::json BehaviorTreeComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_blackBoard)
	{
		auto blackBoardJson_ = m_blackBoard->SerializePrefabData();

		if (!blackBoardJson_.is_null())
		{
			json_["BlackBoard"] = blackBoardJson_;
		}
	}

	if (m_nodeRoot)
	{
		auto nodeRootJson_ = m_nodeRoot->SerializePrefabData();

		if (!nodeRootJson_.is_null())
		{
			json_["NodeRoot"] = nodeRootJson_;
		}
	}

	return json_;
}

void BehaviorTreeComponent::Update()
{
	if (!m_nodeRoot) { return; }

	m_nodeRoot->Tick();
}

void BehaviorTreeComponent::Finalize() const
{

}