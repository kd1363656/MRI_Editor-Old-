#pragma once

#include "../../GameObject/GameObject.h"

#include "../../Scene/SceneManager.h"
#include "../../Scene/BaseScene/BaseScene.h"

namespace ComponentUtility
{
	template <class ComponentType>
		requires std::derived_from<ComponentType , ComponentBase>
	static inline void BoundGUIDComponent(std::weak_ptr<ComponentType>& BoundComponent , uint64_t GUID)
	{
		if (GUID == CommonConstant::UNREGISTERD_GUID) { return; }

		// ゲームオブジェクト全てを捜査して一致する"GUID"のコンポーネントを取得
		if (auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock())
		{
			for (const auto& gameObject_ : scene_->GetGameObjectList())
			{
				if (GUID != gameObject_->GetGUID()) { continue; }

				BoundComponent = gameObject_->GetComponent<ComponentType>();
			}
		}
	}

	template <class ComponentType>
		requires std::derived_from<ComponentType, ComponentBase>
	static inline void SetEnable(std::weak_ptr<ComponentType> Component)
	{
		if (auto component_ = Component.lock())
		{
			component_->Enable();
		}
	}

	template <class ComponentType>
		requires std::derived_from<ComponentType, ComponentBase>
	static inline void SetDisable(std::weak_ptr<ComponentType> Component)
	{
		if (auto component_ = Component.lock())
		{
			component_->Disable();
		}
	}

}