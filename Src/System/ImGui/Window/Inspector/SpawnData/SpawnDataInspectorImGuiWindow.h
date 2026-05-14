#pragma once
#include "../../ImGuiWindowBase.h"

class GameObject;

class SpawnDataInspectorImGuiWindow : public ImGuiWindowBase
{

public:

	SpawnDataInspectorImGuiWindow ()          = default;
	~SpawnDataInspectorImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<SpawnDataInspectorImGuiWindow>(); }

	void Init() override;

	void Update              () override;
	void ComponentInspector  (std::shared_ptr<GameObject> Object) const;
	void AddComponentSelector(std::shared_ptr<GameObject> Object);
	void AddComponentButton  (std::shared_ptr<GameObject> Object) const;

private:

	void ComponentInspector(std::shared_ptr<GameObject> Objectconst , const bool IsSameType) const;

	std::string m_selectedComponentName = "";

};