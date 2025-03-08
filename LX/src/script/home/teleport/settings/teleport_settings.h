#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class teleport_options_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}
