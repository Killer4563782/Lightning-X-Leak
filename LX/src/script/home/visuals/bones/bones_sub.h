#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class bones_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}
