#pragma once

namespace BitShiftUtility
{
	// "std::is_enum_v<EnumType>"は"Enum"型かどうかを評価する
	template <typename EnumType>
	constexpr bool IsValidEnumFlag = std::is_enum_v<EnumType>;

	// コピー渡しのほうが速度が早いが式を
	// 間違って使うリスクを減らすため参照で取得
	template <typename EnumType , typename FlagType>
	void EnableFlag(EnumType FlagToEnable, FlagType& Flags)
	{
		static_assert(IsValidEnumFlag<EnumType>    , "BitShiftUtilityでエラーが出ました、必ずEnum型にしてください"     );
		static_assert(std::is_unsigned_v<FlagType> , "BitShiftUtility : フラグ変数は符号なし整数型である必要があります");

		const auto flag_ = static_cast<FlagType>(FlagToEnable);

		Flags |= flag_;
	}

	template <typename EnumType , typename FlagType>
	void DisableFlag(EnumType FlagToDisable , FlagType& Flags)
	{
		static_assert(IsValidEnumFlag<EnumType>    , "BitShiftUtilityでエラーが出ました、必ずEnum型にしてください"     );
		static_assert(std::is_unsigned_v<FlagType> , "BitShiftUtility : フラグ変数は符号なし整数型である必要があります");

		const auto flag_ = static_cast<FlagType>(FlagToDisable);

		Flags &= ~flag_;
	}

	template <typename EnumType, typename FlagType>
	void ClearFlagsExcept(EnumType KeepFlag, FlagType& Flags)
	{
		static_assert(IsValidEnumFlag<EnumType>   , "BitShiftUtilityでエラーが出ました、必ずEnum型にしてください");
		static_assert(std::is_unsigned_v<FlagType>, "BitShiftUtility : フラグ変数は符号なし整数型である必要があります");

		const auto flag_ = static_cast<FlagType>(KeepFlag);
		Flags &= flag_;
	}

	template <typename EnumType, typename FlagType>
	bool IsStandFlag(EnumType FlagToCheck, FlagType Flags)
	{
		static_assert(IsValidEnumFlag<EnumType>   , "BitShiftUtilityでエラーが出ました、必ずEnum型にしてください");
		static_assert(std::is_unsigned_v<FlagType>, "BitShiftUtility : フラグ変数は符号なし整数型である必要があります");

		const auto flag_ = static_cast<FlagType>(FlagToCheck);

		// フラグが立っていたら"true"を返す
		return (Flags & flag_) != 0;
	}

	// 両者のビットフラグから一つでも一致する場合"true"を返す
	template <typename FlagType>
	bool IsSameFlagStand(FlagType Comp , FlagType Flags)
	{
		static_assert(std::is_unsigned_v<FlagType>, "BitShiftUtility : フラグ変数は符号なし整数型である必要があります");

		// フラグが立っていたら"true"を返す
		return (Flags & Comp) != 0;
	}
}