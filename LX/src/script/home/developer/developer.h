#pragma once

#ifdef ENABLE_DEVELOPER_MODE
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class developer_submenu final : public submenu {
		using submenu::submenu;

		void on_init() override;
	};

	extern developer_submenu g_developer_submenu;
}
#endif