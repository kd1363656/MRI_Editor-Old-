#pragma once
#include "../BTNodeBase.h"

class BTNodeRoot : public BTNodeBase
{

public:

	struct CandidateNodeData
	{
		std::shared_ptr<BTNodeBase> node     = nullptr;
		bool					    isLinked = false;
	};

	BTNodeRoot ()          = default;
	~BTNodeRoot() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeRoot>(); }

	void PostLoadInit() override;
	void Tick        () override;
	
	void ImGuiPrefabDataInspector        () override;
	void ImGuiNodeInspector              () override;
	void ImGuiAfterEndNodeEditorInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;
	
	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	const auto& GetCandidateNodeList    () const { return m_candidateNodeList; }
	auto&       GetWorkCandidateNodeList()		 { return m_candidateNodeList; }
	
	void SetSelectedNode(std::weak_ptr<BTNodeBase> Set) { m_selectedNode = Set; }

	void AddCandidateNodeList(std::shared_ptr<BTNodeBase> Node);

private:

	void ImGuiRootNodeInspector     ();
	void ImGuiCandidateNodeInspector();

	std::shared_ptr<BTNodeRoot> CastSelf();

	std::weak_ptr<BTNodeBase> m_selectedNode;

	std::vector<CandidateNodeData> m_candidateNodeList;

	std::shared_ptr<BTNodeBase> m_childNode = nullptr;
};