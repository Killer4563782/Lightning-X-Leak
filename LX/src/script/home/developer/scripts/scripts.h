#pragma once

#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class scripts_submenu final : public submenu
	{
		using submenu::submenu;

		void on_tick() override;
	};
}
