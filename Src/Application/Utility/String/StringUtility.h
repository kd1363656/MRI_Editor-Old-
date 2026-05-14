#pragma once
#include "../../GameObject/GameObject.h"

#include <regex>

namespace StringUtility
{
	// 多重定義を許す意味も"inline"にはある
	template <typename Class>
	inline std::string StripClassPrefix()
	{
		std::string className_ = typeid(Class).name();

		// "\s*"は戦闘の空白があってもなくても"OK"あった場合削除対象
		// "class"m_typeNameに格納する際に"class Player"の用に代入されるから
		// "\s+"
		std::regex  stripString_(R"(\s*class\s+)");
		className_ = std::regex_replace(className_ , stripString_, "");

		return className_;
	}

	template <typename Struct>
	inline std::string StripStructPrefix()
	{
		std::string structName_ = typeid(Struct).name();

		// "\s*"は戦闘の空白があってもなくても"OK"あった場合削除対象
		// "class"m_typeNameに格納する際に"struct Player"の用に代入されるから
		// "\s+"
		std::regex  stripString_(R"(\s*struct\s+)");
		structName_ = std::regex_replace(structName_, stripString_, "");

		return structName_;
	}
}