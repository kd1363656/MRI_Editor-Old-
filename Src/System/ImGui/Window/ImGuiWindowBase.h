#pragma once

class ImGuiWindowBase
{
public:

	ImGuiWindowBase         () = default;
	virtual ~ImGuiWindowBase() = default;

	virtual uint32_t GetTypeID() const = 0;

	virtual void Init() { /* 必要に応じてオーバーライドしてください */ }

	virtual void Update() = 0;

};