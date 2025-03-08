#include "vehicle_spawner.h"

#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/icon_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "helpers/vehicleHelpers.h"
#include "services/entity_preview/entity_preview.h"
#include "util/vehicle_spawner.h"

namespace ui::submenus
{
	selected_vehicle_class_spawner_submenu::selected_vehicle_class_spawner_submenu(uint8_t selected_vehicle_class, std::string name, std::string description) : submenu(std::move(name), std::move(description)),
		m_selected_vehicle_class(selected_vehicle_class)
	{}

	void selected_vehicle_class_spawner_submenu::on_init()
	{
		for (const auto& vehicles = util::vehicle::g_models.get(static_cast<eVehicleClass>(m_selected_vehicle_class)); const auto& veh : vehicles)
		{
			this->add(function_option(veh.m_name_label, veh.m_manufacturer_label.empty() ? "" : "Manufacturer: " + veh.m_manufacturer_label, [&]
			{
				g_fiber_pool->queue_job([&]
				{
					vehicle::spawn_vehicle(veh.m_model_info->m_hash, self::position, ENTITY::GET_ENTITY_HEADING(self::ped), true, false);
				});
			}));
		}
	}

	void selected_vehicle_class_spawner_submenu::on_tick()
	{
		const auto& vehicle = util::vehicle::g_models.get(static_cast<eVehicleClass>(m_selected_vehicle_class))[m_current];
		entity_preview::vehicle::preview(vehicle.m_model_info->m_hash, vehicle::set_vehicle_maxed_when_spawned);
	}

	void vehicle_settings_submenu::on_init()
	{
		this->add(toggle_option("Sit In Vehicle", "Makes your player sit in the vehicle when spawned", &vehicle::set_ped_in_vehicle_when_spawned));
		this->add(toggle_option("Max Vehicle", "Maxes out all modifications on your newly spawned vehicle", &vehicle::set_vehicle_maxed_when_spawned));
		this->add(toggle_option("Seamless Spawning", "This will make your player seamlessly change vehicles without kicking you out of the previous vehicle, if you were in a vehicle. The newly spawned vehicle will remain at the same speed as the previous vehicle", &vehicle::seamless_re_enter_vehicle));
	}

	void vehicle_spawner_submenu::on_init()
	{
		this->add(vehicle_settings_submenu("Settings"));
		this->add(icon_option("Spawn Vehicle By Name", "Input a vehicle name", ICON_FA_KEYBOARD, [&]
		{
			vehicle::spawn_vehicle_by_name();
		}));
		this->add(break_option("Vehicles"));
		for (uint8_t i = 0; i != static_cast<uint8_t>(23); ++i) {
			this->add(selected_vehicle_class_spawner_submenu(i, util::vehicle::g_model_classes[i], {}));
		}
	}

	vehicle_spawner_submenu g_vehicle_spawner_submenu("Spawner", {});
}
