#pragma once
#include "../BTNodeBase.h"

class BTNodeCompositeBase : public BTNodeBase
{

public:

	BTNodeCompositeBase ()          = default;
	~BTNodeCompositeBase() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeCompositeBase>(); }

	void PostLoadInit() override;

	void OnDestroy() override;

	void ImGuiNodeInspector				 () override;
	void ImGuiAfterEndNodeEditorInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;
	
	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	const auto& GetChildNodeList() const { return m_childNodeList; }

private:

	void AddChildNode(std::shared_ptr<BTNodeBase> Child);

	std::vector<std::shared_ptr<BTNodeBase>> m_childNodeList;

};