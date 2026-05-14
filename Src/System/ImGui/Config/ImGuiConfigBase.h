#pragma once

class ImGuiConfigBase
{

public:

	ImGuiConfigBase         () = default;
	virtual ~ImGuiConfigBase() = default;

	virtual uint32_t GetTypeID() const = 0;

	virtual void Init  () { /* 必要に応じてオーバーライドしてください */ }

	virtual void Update() { /* 必要に応じてオーバーライドしてください */ }

	virtual void Release() { /* 必要に応じてオーバーライドしてください */ }

	virtual void LoadFile() = 0;

	virtual void SaveFile() = 0;

};