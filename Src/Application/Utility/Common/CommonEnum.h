#pragma once

namespace CommonEnum
{

	enum class GameObjectCategoryType
	{
		None   = 0u ,

		Camera = 1 << 0 ,
	
		Player = 1 << 1 ,
		Enemy  = 1 << 2 ,
		Boss   = 1 << 3 ,

		Terrain    = 1 << 4 ,
		BackGround = 1 << 5 ,
		Decoration = 1 << 6 ,
	
		UI = 1 << 7 ,

		Projectile = 1 << 8 ,

		EffekseerEffect = 1 << 9 ,

		Weapon = 1 << 10,
	};

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum class StandardShaderTypeFlags
	{
		None                      = 0u     ,
		Lit                       = 1 << 0 ,
		UnLit                     = 1 << 1 ,
		Bright                    = 1 << 2 ,
		Effect                    = 1 << 3 ,
		UI                        = 1 << 4 ,
		Sprite                    = 1 << 5 ,
		GenerateDepthMapFromLight = 1 << 6 ,
	};

	enum class AddRotationDirectionFlags
	{
		None     = 0u     ,
		Right    = 1 << 0 ,
		Left     = 1 << 1 ,
		Forward  = 1 << 2 ,
		Backward = 1 << 3 
	};

	enum class UseFacingDirectionAxisFlags
	{
	    None = 0u,
	    X    = 1 << 0,
	    Y    = 1 << 1,
	    Z    = 1 << 2,
	};

	enum class ZoomStateFlags
	{
		None    = 0u    ,
		ZoomIn  = 1 << 0,
		ZoomOut = 1 << 1,
	};
	
	enum class RotationStateFlags
	{
		None             = 0u     ,
		RelativeToCamera = 1 << 0 ,
		FaceCamera       = 1 << 1 ,
	};

	enum class BoneType
	{
		None      , 
		RightHand ,
		LeftHand  ,
	};

	enum class ParentType
	{
		None       ,
		GameObject ,
		Model
	};

	enum class EffekseerEffectDeleteType
	{
		None         = 0u     ,
		DeleteSelf   = 1 << 0 ,
		DeleteParent = 1 << 1 
	};

	enum class UseKeyInputFlags
	{
		None    ,
		KeyLeft ,
		KeyRight,
		KeyUp   ,
		KeyDown ,
	};

	enum class UseMouseInputFlags
	{
		ClickMouseLeft,
		ClickMouseRight,
		ClickMouseMiddle
	};

	enum class EventLane : uint16_t
	{
		None   ,
		Moment ,
		Keep   ,
	};

	// "std::unordered_map"などで"bool"型のデフォルト値の"false"を返したくない局面があるから作りました
	enum class BoolFlag
	{
		None  ,
		True  ,
		False ,
	};

	enum class RayCastMode
	{
		Normal   ,  
		ToParent ,
	};

	enum class FadeState
	{
		None    ,
		FadeIn  ,
		Fading  ,
		FadeOut ,
		FadeEnd ,
	};

	enum class Priority
	{
		None  ,
		One   ,
		Two   ,
		Three ,
		Four  ,
		Five  ,
		Six   ,
		Seven ,
		Eight ,
		Nine  ,
		Ten   ,
		Max   ,
	};

	enum class BranchNodeType
	{
		False   , 
		True    ,
		MaxSize ,
	};

	static inline CommonEnum::BoolFlag InvertBoolFlag(CommonEnum::BoolFlag Flag)
	{
		if (Flag == CommonEnum::BoolFlag::True)
		{
			return CommonEnum::BoolFlag::False;
		}
		else if (Flag == CommonEnum::BoolFlag::False)
		{
			return CommonEnum::BoolFlag::True;
		}

		return CommonEnum::BoolFlag::None;
	};
}