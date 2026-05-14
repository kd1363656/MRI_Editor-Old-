#pragma once

class TransformComponent;
class PhysicsProjectileThrowObliqueComponent;
class ComponentCommonCoreAssetFilePath;
class InterpolatorModifierBase;
class BTNodeBase;

namespace CommonStruct
{
	struct StringHash
	{
		// "C++14"以降で追加された"TransparentLookUp"を生かした設計手法
		using is_transparent = void;

		size_t operator()(const std::string& Key) const { return std::hash<std::string>     {}(Key); }
		size_t operator()(std::string_view   Key) const { return std::hash<std::string_view>{}(Key); }
		size_t operator()(const char*        Key) const { return std::hash<std::string_view>{}(Key); }
	};

	struct EventTypeData
	{
		CommonEnum::BoolFlag moment = CommonEnum::BoolFlag::None;
		CommonEnum::BoolFlag keep   = CommonEnum::BoolFlag::None;
	};

	struct ObserverType
	{
		CommonEnum::EventLane eventLane  = CommonEnum::EventLane::None;
		uint32_t              eventTagID = CommonConstant::INVALID_STATIC_ID;
	};

	struct BitShiftList
	{
		const char* label = "";
		uint64_t    type  = 0u;
	};

	struct DirectionList
	{
		const char*   label         = "";
		Math::Vector3 directionType = Math::Vector3::Zero;
	};

	struct PrefabRegistrationData
	{
		std::string prefabName      = "";
		std::string prefabFilePath  = "";
		uint64_t    categoryType    = 0u;
		bool        deleteRequested = false;
	};

	struct PrefabSaveAndLoadFilePath
	{
		std::string filePath = "";
		bool deleteRequested = false;
	};

	struct WindowSize
	{
		int width  = 0;
		int height = 0;
	};

	template <typename EnumType>
	struct EnumList
	{
		const char* label = "";
		EnumType    type  = static_cast<EnumType>(0);
	};

	template <typename ClassType>
	struct GenericClassData
	{
		std::shared_ptr<ClassType> classData = nullptr;
		std::string                name      = CommonConstant::STRING_UNKNOWN;
	};

	struct AnimationReceiveNotify
	{
		uint32_t              eventTagID  = CommonConstant::INVALID_STATIC_ID;
		CommonEnum::EventLane eventLane   = CommonEnum::EventLane::None;
		CommonEnum::BoolFlag  notifyFlag  = CommonEnum::BoolFlag::None;;
		float				  triggerTime = 0.0f;
	};

	struct AnimationTransition
	{
		CommonStruct::AnimationReceiveNotify condition           = {};
		CommonEnum::Priority				 priority            = CommonEnum::Priority::None;
		std::string							 targetAnimationName = CommonConstant::STRING_UNKNOWN;

		bool isTriggered  = false;
		bool isWaitForEnd = false;
	};

	template <class ComponentType>
	struct StrategyData
	{
		std::shared_ptr<IStrategy<ComponentType>> strategy = nullptr;
		std::string								  name     = CommonConstant::STRING_UNKNOWN;
	};

	struct MoveInputActionData
	{
		uint32_t tagID = CommonConstant::INVALID_STATIC_ID;

		CommonStruct::GenericClassData<InterpolatorModifierBase> interpolatorModifier  = {};
		std::shared_ptr<Math::Vector3>							 assignedMoveDirection = nullptr;
	};

	struct LoadChildNodeSpawnData
	{
		std::weak_ptr<BTNodeBase> nodeCache;
		bool					  isLoaded = false;
	};

}