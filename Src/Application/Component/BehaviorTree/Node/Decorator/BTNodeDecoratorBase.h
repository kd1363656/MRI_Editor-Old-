#pragma once
#include "../BTNodeBase.h"

class BTNodeDecoratorBase : public BTNodeBase
{

public:

	BTNodeDecoratorBase ()          = default;
	~BTNodeDecoratorBase() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeDecoratorBase>(); }

	void Init        () override;
	void PostLoadInit() override;
	
	void OnDestroy() override;

	void ImGuiNodeInspector              () override;
	void ImGuiAfterEndNodeEditorInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

private:

	std::shared_ptr<BTNodeBase> m_childNode = nullptr;

protected:

	void TickChild();

	BTNodeBase::NodeState GetChildState();
};