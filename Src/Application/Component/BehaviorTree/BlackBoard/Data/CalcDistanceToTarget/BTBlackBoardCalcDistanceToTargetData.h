#pragma once
#include "../BTBlackBoardDataBase.h"

class BTBlackBoardCalcDistanceToTargetData : public BTBlackBoardDataBase<BTBlackBoardCalcDistanceToTargetData , Math::Vector3>
{

public:

	BTBlackBoardCalcDistanceToTargetData ()          = default;
	~BTBlackBoardCalcDistanceToTargetData() override = default;

	void ImGuiPrefabDataInspector() override 
	{
		ImGui::Text("X : %.2f" , Get().x);
		ImGui::Text("Y : %.2f" , Get().y);
		ImGui::Text("Z : %.2f" , Get().z);
	}
};