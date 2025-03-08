#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class movement_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}
