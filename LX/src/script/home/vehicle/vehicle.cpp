#include "vehicle.h"
#include "vehicle_spawner.h"
#include "commands/manager.hpp"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_bool_option.hpp"
#include "helpers/VehicleHelpers.h"
#include "lx_customs.h"
#include "custom_cars.h"

namespace ui::submenus
{
	void vehicle_submenu::on_init()
	{
		this->add(&g_vehicle_spawner_submenu);
		this->add(&g_lx_customs_submenu);
		this->add(&g_custom_cars_submenu);
		this->add(toggle_option("vehicle_godmode"_LC));
		this->add(toggle_option("sticking_on_ground"_LC));
		this->add(toggle_option("seatbealt"_LC));
		this->add(toggle_option("drift_mode"_LC));
		this->add(vector_bool_option("horn_boost"_LVC));
		this->add(toggle_option("super_brake"_LC));
		this->add(break_option("AI"));
		this->add(function_option("Drive To Waypoint", {}, vehicle::drive_to_waypoint));
		this->add(function_option("Cancel", {}, vehicle::cancel_driving));
		this->add(break_option("Other"));
		this->add(function_option("Max Upgrade", "Fully upgrade current vehicle", [] { { vehicle::max_vehicle(self::vehicle, true); } }));
		this->add(function_option("Performance Upgrade", "Performance wise upgrade current vehicle", [] {g_fiber_pool->queue_job([&] {vehicle::max_vehicle_performance(self::vehicle); }); }));
		this->add(function_option("Downgrade", "Downgrade current vehicle", [] {g_fiber_pool->queue_job([&] { vehicle::downgrade(self::vehicle); }); }));
		this->add(function_option("Repair Vehicle", "Repair current vehicle", [] {g_fiber_pool->queue_job([&] {vehicle::repair_vehicle(self::vehicle); }); }));
		this->add(toggle_option("auto_repair"_LC));
		this->add(function_option("Delete Vehicle", "Delete current vehicle", []{g_fiber_pool->queue_job([&]{vehicle::delete_vehicle(self::vehicle); }); }));


	}

	vehicle_submenu g_vehicle_submenu("Vehicle", {});
}
