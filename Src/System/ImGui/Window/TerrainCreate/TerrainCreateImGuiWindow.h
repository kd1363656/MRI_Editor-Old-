#pragma once
#include "../ImGuiWindowBase.h"

class TerrainCreateImGuiWindow : public ImGuiWindowBase
{

public:

	TerrainCreateImGuiWindow ()          = default;
	~TerrainCreateImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<TerrainCreateImGuiWindow>(); }

	void Update() override;
	
private:

	void UpdateAddGameObjectNum                     ();
	void UpdateAddGameObjectList                    ();
	void UpdateAddGameObjectCreateStartPosition     ();
	void UpdateAddGameObjectGloballyUniqueIdentifier();
	void UpdateAddGameObjectButton                  ();

	std::string m_addGameObjectPrefabName_ = "";

	Math::Vector3 m_createStartPosition = Math::Vector3::Zero;
	Math::Vector3 m_createDirection     = Math::Vector3::Zero;

	float m_createTerrainRadius = 0.0f;

	uint64_t m_boundGameObjectGUID = CommonConstant::UNREGISTERD_GUID;

	int m_createNumber = 0;
};