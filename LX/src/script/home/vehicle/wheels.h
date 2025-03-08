#pragma once
#pragma once
#include "gui/types/submenu.hpp"
namespace ui::submenus
{
	class wheel_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;

	};
	extern wheel_submenu g_wheel_submenu;

}
