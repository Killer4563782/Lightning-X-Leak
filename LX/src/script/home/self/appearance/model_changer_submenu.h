#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class model_changer_submenu final : public submenu {
		using submenu::submenu;

		void on_enter() override;
		void on_init() override;
		void on_tick() override;
	};
}