#include "RawInputManager.h"

#include "../Src/Application/Scene/SceneManager.h"
#include "../Src/Application/main.h"

void RawInputManager::Init()
{
	m_nowKeyStateList.fill  (false);
	m_oldKeyStateList.fill  (false);
	m_nowMouseStateList.fill(false);
	m_oldMouseStateList.fill(false);
}

void RawInputManager::ChangeNowMouseState(Mouse::MouseButtonType Index, bool State)
{
	const int mouseButtonMax_   = static_cast<int>(Mouse::MouseButtonType::MouseButtonMax);
	const int mouseButtonIndex_ = static_cast<int>(Index);

	// マウスキーコードの要素数を超えていたら"return"
	if (mouseButtonIndex_ >= mouseButtonMax_) { return; }

	m_nowMouseStateList[mouseButtonIndex_] = State;
}

void RawInputManager::ResetInputStates()
{
	for (auto& nowKey_ : m_nowKeyStateList)
	{
		nowKey_ = false;
	}

	for(auto& nowMouse_ : m_nowMouseStateList)
	{
		nowMouse_ = false;
	}
}

bool RawInputManager::GetKeyState(int VirtualKeyCode) const
{
	// 仮想キーコードにない値なら"false"を返す
	if (VirtualKeyCode >= CommonConstant::VIRTUAL_KEY_MAX) { return false; }

	return m_nowKeyStateList[VirtualKeyCode];
}

bool RawInputManager::GetNowMouseInputState(int VirtualKeyCode) const
{
	const int mouseButtonMax_ = static_cast<int>(Mouse::MouseButtonType::MouseButtonMax);

	// マウス気コードを超えていたら"return"
	if (VirtualKeyCode >= mouseButtonMax_) { return false; }

	return m_nowMouseStateList[VirtualKeyCode];
}

bool RawInputManager::IsKeyPressedOnce(int VirtualKeyCode) const
{
	// 仮想キーコードにない値なら"false"を返す
	if (VirtualKeyCode >= CommonConstant::VIRTUAL_KEY_MAX) { return false; }

	// もしキーがどちらも格納されていなければキーが初めて押されたことになるから
	// "true"を返す
	const bool isPressNow_ = m_nowKeyStateList[VirtualKeyCode];
	const bool isPressOld_ = m_oldKeyStateList[VirtualKeyCode];
	
	return isPressNow_ && !isPressOld_;
}

bool RawInputManager::IsMouseClickedOnce(int VirtualKeyCode) const
{
	const int mouseButtonMax_ = static_cast<int>(Mouse::MouseButtonType::MouseButtonMax);

	// マウス気コードを超えていたら"return"
	if (VirtualKeyCode >= mouseButtonMax_) { return false; }

	// もしキーがどちらも格納されていなければキーが初めて押されたことになるから
	// "true"を返す
	const bool isPressNowMouse_ = m_nowMouseStateList[VirtualKeyCode];
	const bool isPressOldMouse_ = m_oldMouseStateList[VirtualKeyCode];

	return isPressNowMouse_ && !isPressOldMouse_;
}

void RawInputManager::RegisterDevice()
{
	// m_hWndがしっかりセットされていれば実行
	assert(m_hWnd != NULL);

	RAWINPUTDEVICE rid_[3];

	// デバイスの登録を行っていく
	// キーボード
	rid_[0].usUsagePage = 0x01;		//標準デバイス
	rid_[0].usUsage		= 0x06;		//キーボード
	rid_[0].dwFlags		= RIDEV_INPUTSINK;	
	rid_[0].hwndTarget  = m_hWnd;		

	// マウス
	rid_[1].usUsagePage = 0x01;
	rid_[1].usUsage		= 0x02;
	rid_[1].dwFlags     =RIDEV_INPUTSINK;
	rid_[1].hwndTarget  = m_hWnd;

	// ゲームパッド
	rid_[2].usUsagePage = 0x01;
	rid_[2].usUsage		= 0x05;
	rid_[2].dwFlags		= RIDEV_INPUTSINK;
	rid_[2].hwndTarget  = m_hWnd;

	if(!RegisterRawInputDevices(rid_ , 3 , sizeof(RAWINPUTDEVICE)))
	{
		// std::cerr << "周辺機器の登録に失敗" << std::endl;
	}
}

void RawInputManager::ProcessInput(LPARAM LParam)
{
	UINT dwSize_ = 0;
	GetRawInputData((HRAWINPUT)LParam , RID_INPUT , nullptr , &dwSize_ , sizeof(RAWINPUTHEADER));

	BYTE buffer_[1024];	//最大1024バイトまでの固定バッファにする

	if (dwSize_ == 0             )return;	//何もデータがないときはスキップ
	if (dwSize_ > sizeof(buffer_))return;

	if (GetRawInputData((HRAWINPUT)LParam, RID_INPUT, buffer_, &dwSize_, sizeof(RAWINPUTHEADER)) != dwSize_) { return; }
	
	RAWINPUT* raw_ = reinterpret_cast<RAWINPUT*>(buffer_);

	// キーボード
	if(raw_->header.dwType == RIM_TYPEKEYBOARD)
	{
		int  key_     = raw_->data.keyboard.VKey;
		bool pressed_ = !(raw_->data.keyboard.Flags & RI_KEY_BREAK);

		// キーの入力状態を格納
		// 存在すればそのキーに対応する値を格納
		if (key_ < CommonConstant::VIRTUAL_KEY_MAX)
		{
			m_nowKeyStateList[key_] = pressed_;
		}
	}
	// マウス
	if(raw_->header.dwType == RIM_TYPEMOUSE)
	{
		POINT mousePos_;

		// マウスの座標(デスクトップ全体を対象)を取得
		if (GetCursorPos(&mousePos_))
		{
			// ScreenToClientでマウス座標(デスクトップ全体を対象)を
			// 今のゲームウィンドウ用に変換
			// 左上のを基準に座標を取るので調整する
			ScreenToClient(m_hWnd, &mousePos_);
		}

		WORD flags_ = raw_->data.mouse.usButtonFlags;

		// ボタンの状態を明示的に管理（押下/解放で変更）
		// "RawInput"の仕様上効かない時があるから
		if      (flags_ & RI_MOUSE_MIDDLE_BUTTON_DOWN) { ChangeNowMouseState(Mouse::MouseButtonType::MouseMiddle , true ); }
		else if (flags_ & RI_MOUSE_MIDDLE_BUTTON_UP  ) { ChangeNowMouseState(Mouse::MouseButtonType::MouseMiddle , false); }

		if      (flags_ & RI_MOUSE_LEFT_BUTTON_DOWN) { ChangeNowMouseState(Mouse::MouseButtonType::MouseLeft , true ); }
		else if (flags_ & RI_MOUSE_LEFT_BUTTON_UP  ) { ChangeNowMouseState(Mouse::MouseButtonType::MouseLeft , false); }

		if      (flags_ & RI_MOUSE_RIGHT_BUTTON_DOWN) { ChangeNowMouseState(Mouse::MouseButtonType::MouseRight , true ); }
		else if (flags_ & RI_MOUSE_RIGHT_BUTTON_UP  ) { ChangeNowMouseState(Mouse::MouseButtonType::MouseRight , false); }

		// ホイール
		if(flags_ & RI_MOUSE_WHEEL)
		{
			m_nowMouseData.wheelDelta = static_cast<SHORT>(raw_->data.mouse.usButtonData) / WHEEL_DELTA;
		}
		else
		{
			// 回転がない場合は0
			m_nowMouseData.wheelDelta = 0;
		}
	}

	// ゲームパッド
	if(raw_->header.dwType == RIM_TYPEHID)
	{
		if(raw_->data.hid.dwSizeHid > 0)
		{
			BYTE* data_ = raw_->data.hid.bRawData;

			// ここでゲームパッドのボタン情報を読み取る
			bool buttonA_ = (data_[0] & 0x10) != 0;	//Aボタン仮判定

			if(m_gamepadCallback)
			{
				std::lock_guard<std::mutex> lock_(m_inputMutex);
				m_gamepadCallback(1, buttonA_);	//ボタンA(仮)

				// スティックX軸
				float x_ = (data_[1] - 128.0f) / 128.0f;	// -1.0f~1.0fに変換
				float y_ = (data_[2] - 128.0f) / 128.0f;	// スティックY軸

				// デッドゾーン(小さな揺れを無視する)
				const float deadZone_ = 0.1f;

				if (fabs(x_) < deadZone_) x_ = 0.0f;
				if (fabs(y_) < deadZone_) y_ = 0.0f;

				if(m_gamepadStickCallback) m_gamepadStickCallback(x_ , y_);
				
			}
		}
	}
}

void RawInputManager::Vibrate(float LeftMotor, float RightMotor, int Duration)
{
	XINPUT_VIBRATION vibration_ = {};
	vibration_.wLeftMotorSpeed  = static_cast<WORD>(LeftMotor  * 65535);
	vibration_.wRightMotorSpeed = static_cast<WORD>(RightMotor * 65535);

	// パッド0番(基本は1p)

	// 推定時間後に振動を停止
	Sleep(Duration);

	vibration_.wLeftMotorSpeed  = 0;
	vibration_.wRightMotorSpeed = 0;

	XInputSetState(0 , &vibration_);
}

void RawInputManager::BackUpInputState()
{
	// 現在のフレームでのキー情報を格納する
	for(int ki_ = 0; ki_ < CommonConstant::VIRTUAL_KEY_MAX; ki_++)
	{
		m_oldKeyStateList[ki_] = m_nowKeyStateList[ki_];
	}

	const int mouseButtonMax_ = static_cast<int>(Mouse::MouseButtonType::MouseButtonMax);
	for(int mi_ = 0; mi_ < mouseButtonMax_; mi_++)
	{
		m_oldMouseStateList[mi_] = m_nowMouseStateList[mi_];
	}

	// マウスの情報のバックアップ
	m_oldMouseData.position   = m_nowMouseData.position;
	m_oldMouseData.wheelDelta = m_nowMouseData.wheelDelta;
}