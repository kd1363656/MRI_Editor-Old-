#include "BTNodeWithinDistanceBranch.h"

#include "../../../BlackBoard/BTBlackBoard.h"
#include "../../../BlackBoard/Data/CalcDistanceToTarget/BTBlackBoardCalcDistanceToTargetData.h"

void BTNodeWithinDistanceBranch::ImGuiPrefabDataInspector()
{
	BTNodeBranchBase::ImGuiPrefabDataInspector();

	ImGui::DragFloat("ThresholdDistance" , &m_thresholdDistance , 0.1f);
}

void BTNodeWithinDistanceBranch::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBranchBase::DeserializePrefabData(Json);

	m_thresholdDistance = Json.value("ThresholdDistance" , 0.0f);
}

nlohmann::json BTNodeWithinDistanceBranch::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	// 基底クラスのシリアライズ
	{
		auto baseJson_ = BTNodeBranchBase::SerializePrefabData();
		if (!baseJson_.is_null())
		{
			json_.update(baseJson_);
		}
	}

	json_["ThresholdDistance"] = m_thresholdDistance;

	return json_;
}

bool BTNodeWithinDistanceBranch::IsConditionMatched()
{
	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) 
	{
		return false; 
	}

	auto toTargetDistanceData_ = blackBoardCache_->GetSharedInfo<BTBlackBoardCalcDistanceToTargetData, Math::Vector3>().lock();
	if (!toTargetDistanceData_)
	{
		return false; 
	}

	Math::Vector3 toTargetDistance_ = toTargetDistanceData_->Get();

	// もし範囲内なら"true"をそうでなければ"false"を返す
	if (m_thresholdDistance >= toTargetDistance_.LengthSquared())
	{
		return true;
	}

	return false;
}