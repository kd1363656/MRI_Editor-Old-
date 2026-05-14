#pragma once
#include "../Common/CommonEnum.h"

#include "../BitShift/BitShiftUtility.h"

#include "../../main.h"

#include "../../../System/Input/RawInputManager.h"
#include "../../../System/Calculation/Calculation.h"

namespace InputUtility
{
	static void ApplyDirectionIfKeyPressed(Math::Vector3& CurrentDirection , const Math::Vector3& InputDirection , const CommonEnum::UseKeyInputFlags KeyFlag)
	{
		const auto& rawInputManager_   = RawInputManager::GetInstance				 ();
		auto&		playerInputConfig_ = Application::Instance().GetPlayerInputConfig();

		const int keyCode_ = playerInputConfig_.GetKeyConfigData(KeyFlag);

		// 入力があったなら追加した方角を加算
#ifdef _DEBUG
		if (rawInputManager_.GetKeyState(keyCode_) && !ImGui::IsItemHovered())
#else
		if (rawInputManager_.GetKeyState(keyCode_))
#endif
		{
			Calculation::AddDirection(InputDirection , CurrentDirection);
		}
	}

	// 一フレームだけキー入力を確認
	static bool IsKeyJustPressed(const CommonEnum::UseKeyInputFlags KeyFlag)
	{
		const auto& rawInputManager_   = RawInputManager::GetInstance				 ();
		auto&		playerInputConfig_ = Application::Instance().GetPlayerInputConfig();

		const int keyCode_ = playerInputConfig_.GetKeyConfigData(KeyFlag);

#ifdef _DEBUG
		if (rawInputManager_.IsKeyPressedOnce(keyCode_) && !ImGui::IsItemHovered())
#else
		if (rawInputManager_.IsKeyPressedOnce(keyCode_))
#endif
		{
			return true;
		}

		return false;
	}

	static bool IsKeyHeld(const CommonEnum::UseKeyInputFlags KeyFlag)
	{
		const auto& rawInputManager_ = RawInputManager::GetInstance();
		auto& playerInputConfig_ = Application::Instance().GetPlayerInputConfig();

		const int keyCode_ = playerInputConfig_.GetKeyConfigData(KeyFlag);

#ifdef _DEBUG
		if (rawInputManager_.GetKeyState(keyCode_) && !ImGui::IsItemHovered())
#else
		if (rawInputManager_.GetKeyState(keyCode_))
#endif
		{
			return true;
		}

		return false;
	}

	// 一フレームだけマウス入力を確認
	static bool IsMouseJustClicked(const CommonEnum::UseMouseInputFlags KeyFlag)
	{
		const auto& rawInputManager_   = RawInputManager::GetInstance				 ();
		auto&       playerInputConfig_ = Application::Instance().GetPlayerInputConfig();

		const int keyCode_ = playerInputConfig_.GetMouseConfigData(KeyFlag);

		// TODO
#ifdef _DEBUG
		if (rawInputManager_.IsMouseClickedOnce(keyCode_) && !ImGui::IsItemHovered())
#else
		if (rawInputManager_.IsMouseClickedOnce(keyCode_))
#endif
		{
			return true;
		}

		return false;
	}
}