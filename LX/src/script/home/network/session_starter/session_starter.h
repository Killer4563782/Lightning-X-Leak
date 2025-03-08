#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class session_starter_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern session_starter_submenu g_session_starter_submenu;
}
