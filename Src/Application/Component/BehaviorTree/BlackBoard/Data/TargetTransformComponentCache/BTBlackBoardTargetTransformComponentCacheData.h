#pragma once
#include "../BTBlackBoardDataBase.h"

class TransformComponent;

class BTBlackBoardTargetTransformComponentCacheData : public BTBlackBoardDataBase<BTBlackBoardTargetTransformComponentCacheData , std::shared_ptr<CommonCoreGUIDBinder<TransformComponent>>>
{

public:

	BTBlackBoardTargetTransformComponentCacheData ()          = default;
	~BTBlackBoardTargetTransformComponentCacheData() override = default;

	void Init        () override;
	void PostLoadInit() override;

	void ImGuiSpawnDataInspector () override;
	void ImGuiPrefabDataInspector() override;

	void DeserializeSpawnData (const nlohmann::json& Json) override;
	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializeSpawnData () override;
	nlohmann::json SerializePrefabData() override;

};