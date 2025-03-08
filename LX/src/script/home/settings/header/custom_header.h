#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	inline bool texture_loading_in_progress = false;

	class custom_header_submenu final : public submenu
	{
		using submenu::submenu;

		void on_enter() override;
	};
}
