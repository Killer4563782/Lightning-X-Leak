#include "visuals_sub.h"

#include "bones/bones_sub.h"
#include "boxesp/boxesp_sub.h"
#include "commands/manager.hpp"
#include "commands/settings.h"
#include "gui/types/color_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "names/names_sub.h"
#include "tracers/tracers_sub.h"

namespace ui::submenus
{
	void visuals_submenu::on_init()
	{
		//this->add(boxesp_submenu("Box ESP")); Needs fix
		this->add(tracers_submenu("Tracers"));
		this->add(bones_submenu("Bones"));
		this->add(names_submenu("Names"));
		this->add(toggle_option("player_tags_on_minimap"_BC));
		this->add(toggle_option("draw_waypoint"_LC));
		this->add(color_option("Waypoint Line Color", {}, &commands::settings::visuals::waypoint_color, true));
	}

	visuals_submenu g_visuals_submenu("Visuals", {});
}
