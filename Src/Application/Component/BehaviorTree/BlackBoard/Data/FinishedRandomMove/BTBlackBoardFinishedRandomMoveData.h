#pragma once
#include "../BTBlackBoardDataBase.h"

class BTBlackBoardFinishedRandomMoveData : public BTBlackBoardDataBase<BTBlackBoardFinishedRandomMoveData , bool>
{

public:

	BTBlackBoardFinishedRandomMoveData ()          = default;
	~BTBlackBoardFinishedRandomMoveData() override = default;

	void ImGuiPrefabDataInspector() override;
};