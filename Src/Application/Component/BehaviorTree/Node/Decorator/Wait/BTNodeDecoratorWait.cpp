#include "BTNodeDecoratorWait.h"

#include "../../../../../main.h"

void BTNodeDecoratorWait::Init()
{
	BTNodeDecoratorBase::Init();

	m_waitCnt  = 0.0f;
	m_waitTime = CommonConstant::FLOAT_ONE;
}

void BTNodeDecoratorWait::Tick()
{
	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

	// 所定時間まで待機中なら時間を加算
	if (m_waitCnt < m_waitTime)
	{
		m_waitCnt += deltaTime_;
		SetNodeState(BTNodeBase::NodeState::Fail);
	}
	else
	{
		// 所定時間をオーバーしたらカウンターを初期化
		m_waitCnt = 0.0f;

		// 子ノードを実行して成功したら成功を返す
		TickChild();
		SetNodeState(GetChildState());
	}
}

void BTNodeDecoratorWait::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat("WaitCnt"  , &m_waitCnt  , 0.1f);
	ImGui::DragFloat("WaitTime" , &m_waitTime , 0.1f);
}