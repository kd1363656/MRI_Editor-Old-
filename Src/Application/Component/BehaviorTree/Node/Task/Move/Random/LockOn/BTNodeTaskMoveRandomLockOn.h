#pragma once
#include "../../../BTNodeTaskBase.h"

#include "../../../../../../../Utility/Common/CommonStruct.h"

class InterpolatorModifierBase;
class CommonCoreMoveInput;

class BTNodeTaskMoveRandomLockOn final : public BTNodeTaskBase
{

public:

	enum class MoveDirection
	{
		Right    ,
		Left     ,
		Forward  ,
		Backward ,
		MaxSize  ,
	};

	BTNodeTaskMoveRandomLockOn ()          = default;
	~BTNodeTaskMoveRandomLockOn() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeTaskMoveRandomLockOn>(); }

	void Init        () override;
	void PostLoadInit() override;
	void Tick        () override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	static constexpr float NORMAL_WEIGHT = 1.0f;
	static constexpr float HIGH_WEIGHT   = 3.0f;

	void InitRandDirectionWeightList    (BTNodeTaskMoveRandomLockOn::MoveDirection Index);
	void ChangeWeightByPrevMoveDirection();
	void ResetWeight						();
	void FixMoveDirection				(BTNodeTaskMoveRandomLockOn::MoveDirection Direction , Math::Vector3& MoveDirection);

	void FixMoveRandomDirection();
	void Rotation              ();
	void Move                  ();

	std::array<CalculationRandom::RandomWeightData<int>, static_cast<int>(BTNodeTaskMoveRandomLockOn::MoveDirection::MaxSize)> m_randDirectionWeightList;

	std::shared_ptr<CommonCoreMoveInput> m_commonCoreMoveInput = nullptr;

	CommonStruct::GenericClassData<InterpolatorModifierBase> m_interpolatorModifier;

	BTNodeTaskMoveRandomLockOn::MoveDirection m_prevMoveDirection = BTNodeTaskMoveRandomLockOn::MoveDirection::MaxSize;

	float m_directionChangeElapsedTimer = 0.0f;
	float m_directionFixTime            = 0.0f;

	int m_directionChangeCnt    = 0;
	int m_maxDirectionChangeCnt = 0;

	bool m_isDirectionChangeTimerInitialized = false;
};