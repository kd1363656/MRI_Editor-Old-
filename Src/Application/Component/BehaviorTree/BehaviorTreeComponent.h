#pragma once

class BTBlackBoard;
class BTNodeRoot;

class BehaviorTreeComponent : public ComponentBase
{

public:

	BehaviorTreeComponent ()          = default;
	~BehaviorTreeComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<BehaviorTreeComponent>(); }

	void Init        () override;
	void PostLoadInit() override;
	
	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

	void Update     () override;
	void Finalize   () const;

private:

	std::shared_ptr<BTBlackBoard> m_blackBoard = nullptr;
	std::shared_ptr<BTNodeRoot>	  m_nodeRoot   = nullptr;
};