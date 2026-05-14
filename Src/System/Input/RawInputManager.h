#pragma once
#include "../Src/Application/Utility/Singleton/SingletonBase.h"

#include <Xinput.h>
#pragma comment(lib , "xinput.lib")	//XInputライブラリリンク

namespace Mouse
{
	// マウスの仮想キーコードは"1" ~ "6"まで
	enum class MouseButtonType 
	{
		None			 ,
		MouseLeft        ,
		MouseRight       ,
		MouseMiddle      ,
		MouseX1          ,
		MouseX2          ,
		MouseButtonMax
	};

	struct Data
	{
		Math::Vector2 position   = Math::Vector2::Zero;
		int			  wheelDelta = 0;
	};
}

// キーボードの入力は保証
class RawInputManager : public SingletonBase<RawInputManager>
{
public:

	void Init();
	
	void ChangeNowMouseState(Mouse::MouseButtonType Index , bool State);

	void ResetInputStates();

	void SetHWnd(HWND Set) { m_hWnd = Set; }

	// ゲームパッドスティック用のコールバック
	void SetGamepadStickCallback(std::function<void(float x, float y)> callback)
	{
		m_gamepadStickCallback = callback;
	}

	const std::array<bool , CommonConstant::VIRTUAL_KEY_MAX>& GetNowKeyStateList()const { return m_nowKeyStateList; }
	const std::array<bool , CommonConstant::VIRTUAL_KEY_MAX>& GetOldKeyStateList()const { return m_oldKeyStateList; }

	const Mouse::Data& GetNowMouseData() const { return m_nowMouseData; }

	bool GetKeyState          (int VirtualKeyCode) const;
	bool GetNowMouseInputState(int VirtualKeyCode) const;

	bool IsKeyPressedOnce  (int VirtualKeyCode) const;
	bool IsMouseClickedOnce(int VirtualKeyCode) const;

	void RegisterDevice();
	void ProcessInput  (LPARAM LParam);
	void Vibrate       (float LeftMotor , float RightMotor , int Duration);

	void BackUpInputState();

private:

	HWND m_hWnd = NULL;

	// マルチスレッド対応用
	// すべての入力で共通の"mutex"
	std::mutex m_inputMutex;

	// キーの入力状態を毎フレーム格納するリスト
	std::array<bool , CommonConstant::VIRTUAL_KEY_MAX> m_nowKeyStateList;
	std::array<bool , CommonConstant::VIRTUAL_KEY_MAX> m_oldKeyStateList;

	// マウスの入力状態を毎フレーム格納するリスト
	std::array<bool, static_cast<int>(Mouse::MouseButtonType::MouseButtonMax)> m_nowMouseStateList;
	std::array<bool, static_cast<int>(Mouse::MouseButtonType::MouseButtonMax)> m_oldMouseStateList;

	// マウスの入力状態、座標を舞フレーム格納
	Mouse::Data m_nowMouseData;
	Mouse::Data m_oldMouseData;

	std::function<void(int   Button, bool  Pressed)> m_gamepadCallback;		 //ゲームパッド用コールバック
	std::function<void(float X     , float Y      )> m_gamepadStickCallback; //ゲームパッドのスティック用のコールバック

	//=====================================================
	// シングルトンパターン
	//=====================================================
	friend class SingletonBase;

	RawInputManager ()          = default;
	~RawInputManager() override = default;

};