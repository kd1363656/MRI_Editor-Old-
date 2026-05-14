#pragma once

class GameObject;
class BTNodeRoot;
class BTBlackBoard;

class BTNodeBase : public std::enable_shared_from_this<BTNodeBase>
{

public:

	enum class NodeState
	{
		Idle    ,
		Running	,
		Success	,
		Fail	
	};

	BTNodeBase         () = default;
	virtual ~BTNodeBase() = default;

	virtual uint32_t GetTypeID() const = 0;

	virtual void Init        ();
	virtual void PostLoadInit();
	virtual void Tick        () = 0;
	
	virtual void OnDestroy();

	virtual void DeserializeSpawnData (const nlohmann::json& Json);
	virtual void DeserializePrefabData(const nlohmann::json& Json);
	void		 DeserializeGUID	  (const nlohmann::json& Json);

	virtual nlohmann::json SerializeSpawnData ();
	virtual nlohmann::json SerializePrefabData();

	virtual void ImGuiSpawnDataInspector         () { /* 必要に応じてオーバーライドしてください */ }
	virtual void ImGuiPrefabDataInspector        () { /* 必要に応じてオーバーライドしてください */ }
	virtual void ImGuiNodeInspector              ();
	virtual void ImGuiAfterEndNodeEditorInspector();
	
	std::weak_ptr<BTBlackBoard> GetBlackBoardCache() const { return m_blackBoardCache; }
	std::weak_ptr<BTNodeRoot>   GetNodeRootCache  () const { return m_nodeRootCache;   }
	
	std::string_view GetNodeName() const { return m_nodeName; }

	BTNodeBase::NodeState GetNodeState() const { return m_nodeState; }

	ImVec2 GetNodePos() const { return m_nodePos; }

	uint64_t GetGUID() const { return m_guid; }

	void SetBlackBoardCache(std::weak_ptr<BTBlackBoard> Set) { m_blackBoardCache = Set; }
	void SetNodeRootCache  (std::weak_ptr<BTNodeRoot>   Set) { m_nodeRootCache   = Set; }
	
	void SetNodeName(const std::string& Set) { m_nodeName = Set; }

	void SetNodeState(const BTNodeBase::NodeState Set) { m_nodeState = Set; }

	void SetGUID(uint64_t Set) { m_guid = Set; }

	void SetNodePos(const ImVec2 Set) { m_nodePos = Set; }

private:

	std::weak_ptr<BTNodeRoot>   m_nodeRootCache;
	std::weak_ptr<BTBlackBoard> m_blackBoardCache;
	
	std::string m_nodeName = CommonConstant::STRING_UNKNOWN;

	BTNodeBase::NodeState m_nodeState = BTNodeBase::NodeState::Idle;

	uint64_t m_guid = CommonConstant::UNREGISTERD_GUID;

	ImVec2 m_nodePos = {};
	
};