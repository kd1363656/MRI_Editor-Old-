#pragma once
#include "../Common/CommonStruct.h"
#include "../../Tag/ComponentEvent/ComponentEventTag.h"
#include "../../Tag/TagRegistry.h"

namespace JsonUtility
{
	// "Json"のヌルチェックなどで便利な関数
	inline bool CheckIsContainAndArray(const nlohmann::json& Json, const std::string& Key)
	{
		return Json.contains(Key) && Json[Key].is_array();
	}

	// "Json"変換用便利関数
	inline Math::Vector2  JsonToVec2(const nlohmann::json& Json) { return Math::Vector2  { Json.value("X"   , 0.0f) , Json.value("Y" , 0.0f) };   }
	inline nlohmann::json Vec2ToJson(const Math::Vector2&  Vec2) { return nlohmann::json { { "X" , Vec2.x } ,                   {"Y" , Vec2.y} }; }

	inline Math::Vector3  JsonToVec3(const nlohmann::json& Json ) { return Math::Vector3 { Json.value("X" , 0.0f) , Json.value("Y" , 0.0f) , Json.value("Z" , 0.0f) }; }
	inline nlohmann::json Vec3ToJson(const Math::Vector3&  Value) { return nlohmann::json{ {"X" , Value.x}        , {"Y" , Value.y}        , {"Z" , Value.z}	    }; }

	inline Math::Vector4  JsonToVec4(const nlohmann::json& Json) { return Math::Vector4 { Json.value("X" , 0.0f) , Json.value("Y" , 0.0f) , Json.value("Z" , 0.0f) , Json.value("W" , 0.0f) }; }
	inline nlohmann::json Vec4ToJson(const Math::Vector4&  Vec4) { return nlohmann::json{ {"X" , Vec4.x }        , {"Y" , Vec4.y }        , {"Z" , Vec4.z }        , {"W" , Vec4.w}         }; }

	inline ImVec2         JsonToImVec2(const nlohmann::json& Json)   { return ImVec2        { Json.value("X" , 0.0f) , Json.value("Y" , 0.0f) }; }
	inline nlohmann::json ImVec2ToJson(const ImVec2&         ImVec2) { return nlohmann::json{ {"X" , ImVec2.x }      , {"Y" , ImVec2.y }      }; }

	// クオータニオンは単位クオータニオンである必要があるため少し特殊な関数です
	inline Math::Quaternion JsonToQuaternion(const nlohmann::json&   Json      ) { return Math::Quaternion{ Json.value("X" , 0.0f) , Json.value("Y" , 0.0f) , Json.value("Z" , 0.0f) , Json.value("W" , Math::Quaternion::Identity.w) }; }
	inline nlohmann::json   QuaternionToJson(const Math::Quaternion& Quaternion) { return nlohmann::json  { {"X" , Quaternion.x }  , {"Y" , Quaternion.y }  , {"Z" , Quaternion.z }  , {"W" , Quaternion.w}							  }; }

	inline Math::Color    JsonToColor(const nlohmann::json& Json ) { return Math::Color    { Json.value("X"   , 0.0f) , Json.value("Y" , 0.0f) , Json.value("Z"  , 0.0f) , Json.value("W" , 0.0f) }; }
	inline nlohmann::json ColorToJson(const Math::Color&    Color) { return nlohmann::json { { "X" , Color.x }        , {"Y" , Color.y}        , {"Z" , Color.z} , {"W" , Color.w  }              }; }

	inline CommonStruct::WindowSize JsonToWindowSize(const nlohmann::json           Json  ) { return CommonStruct::WindowSize{  Json.value("Width" , 0)   , {Json.value("Height" , 0)} }; }
	inline nlohmann::json           WindowSizeToJson(const CommonStruct::WindowSize& Value) { return nlohmann::json          {  { "Width" , Value.width } , {"Height" , Value.height } }; }

	// 文字列を"ID"に変換
	inline uint32_t JsonToComponentEventTagID(const nlohmann::json& Json , const char* Key)
	{
		auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

		std::string eventTagName_ = Json.value(Key , CommonConstant::STRING_UNKNOWN);
		return tagRegistry_.GetTagID(eventTagName_);
	}

	// "ID"を文字列に変換
	inline void ComponentEventTagIDToJson(nlohmann::json& Json , const uint32_t ID , const char* Key)
	{
		auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

		std::string eventTagName_ = tagRegistry_.GetTagName(ID);
		Json[Key] = eventTagName_;
	}

	template <typename ConvertValue, typename EnumType>
		requires std::is_enum_v<EnumType>
	inline EnumType JsonToEnum(EnumType InitValue , const nlohmann::json& Json , const char* Key)
	{
		ConvertValue value_ = Json.value(Key , static_cast<ConvertValue>(InitValue));
		return static_cast<EnumType>(value_);
	}
}