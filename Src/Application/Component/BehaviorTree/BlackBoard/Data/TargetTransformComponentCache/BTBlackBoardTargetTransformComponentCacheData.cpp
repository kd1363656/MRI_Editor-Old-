#include "BTBlackBoardTargetTransformComponentCacheData.h"

#include "../../../../Transform/TransformComponent.h"

void BTBlackBoardTargetTransformComponentCacheData::Init()
{
	// インスタンスが生成されていなければ初期化
	if (!Get())
	{
		auto created_ = std::make_shared<CommonCoreGUIDBinder<TransformComponent>>();
		Set(created_);
	}

	Get()->Init();
}
void BTBlackBoardTargetTransformComponentCacheData::PostLoadInit()
{
	auto instance_ = Get();
	if (!instance_) { return; }

	instance_->PostLoadInit();
}

void BTBlackBoardTargetTransformComponentCacheData::ImGuiSpawnDataInspector()
{
	auto instance_ = Get();
	if (!instance_) { return; }

	instance_->ImGuiSpawnDataInspector();
}
void BTBlackBoardTargetTransformComponentCacheData::ImGuiPrefabDataInspector()
{
	auto instance_ = Get();
	if (!instance_) { return; }

	instance_->ImGuiPrefabDataInspector();
}

void BTBlackBoardTargetTransformComponentCacheData::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	auto instance_ = Get();
	if (!instance_) { return; }

	instance_->DeserializeSpawnData(Json);
}
void BTBlackBoardTargetTransformComponentCacheData::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	auto instance_ = Get();
	if (!instance_) { return; }

	instance_->DeserializePrefabData(Json);
}

nlohmann::json BTBlackBoardTargetTransformComponentCacheData::SerializeSpawnData()
{
	auto json_ = nlohmann::json();
	if (auto instance_ = Get())
	{
		auto boundJson_ = instance_->SerializeSpawnData();

		if (!boundJson_.is_null())
		{
			json_.update(boundJson_);
		}
	}
	
	return json_;
}
nlohmann::json BTBlackBoardTargetTransformComponentCacheData::SerializePrefabData()
{
	auto json_ = nlohmann::json();
	if (auto instance_ = Get())
	{
		auto boundJson_ = instance_->SerializePrefabData();

		if (!boundJson_.is_null())
		{
			json_.update(boundJson_);
		}
	}
	
	return json_;
}