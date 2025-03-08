#pragma once
#ifdef ENABLE_DEVELOPER_MODE
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class particle_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}
#endif