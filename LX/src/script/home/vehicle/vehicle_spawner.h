#pragma once
#include "gui/types/submenu.hpp"
#include "util/vehicle_spawner.h"

namespace ui::submenus
{
	class selected_vehicle_class_spawner_submenu final : public submenu
	{
	public:
		uint8_t m_selected_vehicle_class = 0;

		explicit selected_vehicle_class_spawner_submenu(uint8_t selected_vehicle_class, std::string name, std::string description);

		void on_init() override;
		void on_tick() override;
	};

	class vehicle_spawner_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	class vehicle_settings_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern vehicle_spawner_submenu g_vehicle_spawner_submenu;
}
