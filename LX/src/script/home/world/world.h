#pragma once
#include "gui/types/number_option.hpp"
#include "gui/types/submenu.hpp"
#include "gui/types/vector_option.hpp"

namespace ui::submenus
{
	class time_option final : public number_option<int>
	{
		using number_option::number_option;

		void action(ActionType type, bool first_receiver) override;
	};


	class weather_option final : public vector_option
	{
		using vector_option::vector_option;

		void action(ActionType type, bool first_receiver) override;
	};


	class world_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern world_submenu g_world_submenu;
}
