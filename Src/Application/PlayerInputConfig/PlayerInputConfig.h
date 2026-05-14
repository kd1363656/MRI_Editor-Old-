#pragma once
#include "../Utility/Json/JsonUtility.h"

template <typename EnumType>
constexpr bool IsValidEnumFlag = std::is_enum_v<EnumType>;

// ゲームをプレイするうえで使う頻度の高いキーをまとめたもの
// 入力管理コンポーネントが多いと
class PlayerInputConfig
{

public:

	PlayerInputConfig () = default;
	~PlayerInputConfig() = default;

	void Init();

	void LoadConfig();
	void SaveConfig();

	const auto& GetKeyConfigList  () const { return m_keyConfigList;   }
	const auto& GetMouseConfigList() const { return m_mouseConfigList; }

	int GetKeyConfigData  (CommonEnum::UseKeyInputFlags   Key  );
	int GetMouseConfigData(CommonEnum::UseMouseInputFlags Mouse);

	template <typename EnumType>
	void DeserializeConfig(const char* Key , const nlohmann::json& Json , std::unordered_map<EnumType, int>& ConfigList) const
	{
		static_assert(IsValidEnumFlag<EnumType>, "PlayerInputConfigでエラーが出ました、必ずEnum型にしてください");

		// リストに読み取り結果を反映
		if (JsonUtility::CheckIsContainAndArray(Json, Key))
		{
			for (const auto& array_ : Json[Key])
			{
				const int configKey_      = array_.value("Config"        , 0);
				const int virtualKeyCode_ = array_.value("VirtualKeyCode", 0);

				ConfigList.emplace(static_cast<EnumType>(configKey_), virtualKeyCode_);
			}
		}
	}

	template <typename EnumType>
	nlohmann::json SerializeConfig(const char* Key , std::unordered_map<EnumType, int>& ConfigList)
	{
		static_assert(IsValidEnumFlag<EnumType>, "PlayerInputConfigでエラーが出ました、必ずEnum型にしてください");

		auto keyArray_ = nlohmann::json::array();
		auto rootJson_ = nlohmann::json       ();

		for (const auto& [key_, value_] : ConfigList)
		{
			auto json_ = nlohmann::json();
			json_["Config"        ] = static_cast<uint64_t>(key_);
			json_["VirtualKeyCode"] = value_;

			// キー情報を追加
			keyArray_.emplace_back(json_);
		}

		rootJson_[Key] = keyArray_;

		return rootJson_;
	}

private:

	const char* INPUT_CONFIG_FILE_PATH = "Asset/Data/PlayerInputConfig/PlayerInputConfig";

	std::unordered_map<CommonEnum::UseKeyInputFlags   , int> m_keyConfigList;
	std::unordered_map<CommonEnum::UseMouseInputFlags , int> m_mouseConfigList;
};