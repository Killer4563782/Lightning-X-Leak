#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class vision_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern vision_submenu  g_vision_submenu;
}