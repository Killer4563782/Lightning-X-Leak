#include "self.h"

#include "appearance/appearance.h"
#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/number_bool_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "movement/movement.h"

namespace ui::submenus
{
	void self_submenu::on_init()
	{
		this->add(appearance_submenu("Appearance", "Change the way you appear"));
		this->add(movement_submenu("Movement"));
		this->add(toggle_option("godmode"_LC));
		this->add(number_bool_option<int>("lock_wanted_level"_LIC));
		this->add(toggle_option("no_ragdoll"_LC));
		this->add(number_bool_option<float>("free_cam"_LFC)); // Move to world section
		this->add(number_bool_option<float>("no_clip"_LFC));
		this->add(toggle_option("fast_respawn"_LC));
		this->add(toggle_option("inf_oxygen"_LC));
		this->add(number_bool_option<float>("health_regen"_LFC));
		this->add(function_option("max_health"_C));
		this->add(function_option("max_armor"_C));
		this->add(function_option("suicide"_C));
	}

	self_submenu g_self_submenu("Self", "Options for yourself");
}
