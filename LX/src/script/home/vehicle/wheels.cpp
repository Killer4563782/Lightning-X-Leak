#include <util/script_local.hpp>
#include <gui/types/function_option.hpp>
#include <rage/util.h>
#include "wheels.h"
#include <gui/types/number_option.hpp>
#include "commands/settings.h"
using namespace commands::settings;

namespace ui::submenus
{
	void wheel_submenu::on_init()
	{
		for (const auto& [wheels, name] : tables::wheels)
		{
			this->add(function_option(name, name, [&]
			{
				g_fiber_pool->queue_job([&]
				{
					Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
					if (veh != 0) {
						VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); 
						VEHICLE::SET_VEHICLE_WHEEL_TYPE(veh, wheels); 
					};

				});
				
			}));
			
		}
		
		this->add(number_option("Wheels", {}, &lx_customs::wheels, 200, 0, 1, [this] () 
		{
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
			{
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			}
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_FRONTWHEELS, lx_customs::wheels, 0);
		}));
	}
	wheel_submenu g_wheel_submenu(" wheels", {});
}