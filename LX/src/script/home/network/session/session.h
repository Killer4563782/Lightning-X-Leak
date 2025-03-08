#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class session_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
		void on_tick() override;
	};

	extern session_submenu g_session_submenu;
}
