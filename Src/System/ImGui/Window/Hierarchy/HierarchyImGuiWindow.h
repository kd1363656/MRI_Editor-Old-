#pragma once
#include "../ImGuiWindowBase.h"

class GameObject;

class HierarchyImGuiWindow : public ImGuiWindowBase
{

public:

	HierarchyImGuiWindow ()          = default;
	~HierarchyImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<HierarchyImGuiWindow>(); }

	void Update                                () override;
	void AddGameObjectButton             ();
	void GameObjectShowSelector          ();
	void LinkGameObjectFamilyRelationShip(std::weak_ptr<GameObject> GameObject);
	void CancellationFamilyRelationShip  ();

	std::weak_ptr<GameObject> GetInspectorWindowTargetGameObject() const { return m_inspectorWindowTargetGameObject; }

private:

	bool IsDescendant(const std::weak_ptr<GameObject> Parent , const std::weak_ptr<GameObject> PotentialChild);

	std::weak_ptr<GameObject> m_inspectorWindowTargetGameObject;
	std::weak_ptr<GameObject> m_currentDraggingGameObject;

	std::string m_addGameObjectPrefabName_ = "";

	bool m_hasDroppedBlank = false;
	
};