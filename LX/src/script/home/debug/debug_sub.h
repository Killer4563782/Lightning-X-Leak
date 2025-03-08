#pragma once
#ifdef _DEBUG 
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class debug_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern debug_submenu g_debug_submenu;
}
#endif