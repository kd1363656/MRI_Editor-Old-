#pragma once
#include "../BTNodeBase.h"

class BTNodeBranchBase : public BTNodeBase
{

public:

	BTNodeBranchBase ()          = default;
	~BTNodeBranchBase() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeBranchBase>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Tick() override;

	void OnDestroy() override;

	void ImGuiNodeInspector				 () override;
	void ImGuiAfterEndNodeEditorInspector() override;

	void DeserializeSpawnData           (const nlohmann::json& Json) override;
	void DeserializePrefabData          (const nlohmann::json& Json) override;
	
	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	const auto& GetChildNodeList() const { return m_childNodeList; }

private:

	void DeserializeBranchNodeSpawnData (const nlohmann::json& Json , CommonEnum::BranchNodeType , const char* NodeKey);
	void DeserializeBranchNodePrefabData(const nlohmann::json& Json , CommonEnum::BranchNodeType , const char* NodeNameKey , const char* NodeKey);

	nlohmann::json SerializeBranchNodeSpawnData (CommonEnum::BranchNodeType NodeIndex , const char* NodeKey);
	nlohmann::json SerializeBranchNodePrefabData(CommonEnum::BranchNodeType NodeIndex , const char* NodeNameKey , const char* NodeKey);

	void ChangeChildNodeType(const int Index , std::shared_ptr<BTNodeBase> Child);
	void LinkChildNode      (std::shared_ptr<BTNodeBase> Child);

	// ブランチノードは二つ以上ノードがいらない
	std::array<std::shared_ptr<BTNodeBase> , static_cast<int>(CommonEnum::BranchNodeType::MaxSize)> m_childNodeList;

	CommonEnum::BranchNodeType m_nowConditionMatchedIndex = CommonEnum::BranchNodeType::False;
	
protected:

	virtual bool IsConditionMatched() = 0;

};