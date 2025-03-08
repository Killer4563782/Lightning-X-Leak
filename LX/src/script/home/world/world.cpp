#include "world.h"

#include "commands/manager.hpp"
#include "commands/settings.h"
#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/color_option.hpp"
#include "gui/types/icon_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_option.hpp"

using namespace commands::settings;

namespace ui::submenus
{
	void time_option::action(ActionType type, bool first_receiver)
	{
		if (type == ActionType::Enter)
		{
			NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(time::hours, time::minutes, time::seconds);
		}
		else number_option::action(type, first_receiver);
	}


	void weather_option::action(ActionType type, bool first_receiver)
	{
		if (type == ActionType::Enter)
		{
			std::vector<std::string> args;
			"set_weather"_VC->call(args);
		}
		else vector_option::action(type, first_receiver);
	}


	void world_submenu::on_init()
	{
		this->add(break_option("Custom Light"));
		this->add(toggle_option("custom_lights"_LC));
		this->add(color_option("Color", {}, &custom_lights::color, false));
		this->add(number_option("Range", "Starts to bug out at 9400", &custom_lights::range, 9999.f, 0.f, 10.f));
		this->add(number_option("Intensity", {}, &custom_lights::intensity, 99999.f, 0.f, 10.f));
		this->add(break_option("Sky"));
		this->add(toggle_option("Enable Sky Color", {},&g_vfx.enable_custom_sky_color));
		this->add(color_option("Azimuth East Color", {}, &g_vfx.azimuth_east));
		this->add(color_option("Azimuth West Color", {}, &g_vfx.azimuth_west));
		this->add(color_option("Azimuth Transition Color", {}, &g_vfx.azimuth_transition));
		this->add(color_option("Zenith Color", {}, &g_vfx.zenith));
		this->add(toggle_option("rainbow_sky"_LC));
		this->add(break_option("World"));
		this->add(weather_option("set_weather"_VC));
		this->add(time_option("Hour", "[0 - 23]", &time::hours, 23, 0, 1));
		this->add(time_option("Minutes", "[0 - 59]", &time::minutes, 59, 0, 1));
		this->add(time_option("Seconds", "[0 - 59]", &time::seconds, 59, 0, 1));
		this->add(break_option("Misc"));
		this->add(toggle_option("show_area"_LC));
#ifndef _DEBUG
		this->add(icon_option("clear_area"_C, ICON_FA_TRASH_CAN));
#endif
	}

	world_submenu g_world_submenu("World", "Options to change the world locally");
}