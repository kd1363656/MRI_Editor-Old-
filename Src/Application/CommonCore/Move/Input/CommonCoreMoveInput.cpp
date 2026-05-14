#include "CommonCoreMoveInput.h"
#include "../CommonCoreMove.h"

#include "../../../GameObject/GameObject.h"

#include "../../../main.h"

void CommonCoreMoveInput::Init()
{
	if (!m_commonCoreMove)
	{
		m_commonCoreMove = std::make_shared<CommonCoreMove>();
	}

	m_inputMoveDirection = Math::Vector3::Zero;
}
void CommonCoreMoveInput::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	if (!Owner) { return; }

	if (m_commonCoreMove)
	{
		m_commonCoreMove->PostLoadInit(Owner);
	}
}