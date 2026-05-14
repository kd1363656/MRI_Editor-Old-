#pragma once

namespace GlobalEvent
{
	enum class KeyEvent : uint16_t
	{
		PressRight,
		PressLeft ,
		PressUp   ,
		PressDown ,
		Max       ,
	};

	enum class MouseEvent : uint16_t
	{
		ClickLeft ,
		Max		  ,
	};
}