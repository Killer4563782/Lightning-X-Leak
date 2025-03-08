#include "teleport.h"
#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/vector_bool_option.hpp"
#include "helpers/teleportHelpers.h"
#include "settings/teleport_settings.h"

namespace ui::submenus
{
	void teleport_submenu::on_init()
	{
		this->add(teleport_options_submenu("Teleport Settings"));
		this->add(function_option("Teleport To Waypoint", "Teleport To Waypoint", teleport::teleport_to_waypoint));
		this->add(function_option("Teleport To Objective", "Teleport To Objective", teleport::teleport_to_objective));
		this->add(vector_bool_option("auto_teleport"_LVC));
	}

	teleport_submenu g_teleport_submenu("Teleport", {});
}
