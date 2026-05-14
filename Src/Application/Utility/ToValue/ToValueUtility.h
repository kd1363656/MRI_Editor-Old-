#pragma once

namespace ToValueUtility
{
	template <typename ConvertValue , typename EnumType>
		requires std::is_enum_v<EnumType>
	static constexpr ConvertValue EnumToValue(EnumType Enum)
	{
		return static_cast<ConvertValue>(Enum);
	}
}