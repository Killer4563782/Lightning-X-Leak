#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class give_collectibles_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}