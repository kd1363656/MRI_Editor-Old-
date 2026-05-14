#pragma once

class RenderShaderParamBase
{

public:

	RenderShaderParamBase         () = default;
	virtual ~RenderShaderParamBase() = default;

	virtual void Init() = 0;

	virtual void EnableParamEffect () = 0;
	virtual void DisableParamEffect() = 0;
	
	virtual void ImGuiPrefabDataInspector() { /* 必要に応じてオーバーライドしてください */ }

	virtual void DeserializePrefabData(const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }

	virtual nlohmann::json SerializePrefabData() { return nlohmann::json(); }

};