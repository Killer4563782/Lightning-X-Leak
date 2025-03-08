#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class spoofing_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern spoofing_submenu g_spoofing_submenu;
}
