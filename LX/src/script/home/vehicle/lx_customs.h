#pragma once
#include "gui/types/submenu.hpp"
namespace ui::submenus
{
	class lx_customs_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern lx_customs_submenu g_lx_customs_submenu;
}
