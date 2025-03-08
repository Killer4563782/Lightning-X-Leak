#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class walk_style_submenu final : public submenu {
		using submenu::submenu;

		void on_init() override;
	};
}