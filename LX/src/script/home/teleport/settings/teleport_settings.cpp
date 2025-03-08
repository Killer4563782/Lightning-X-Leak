#include "teleport_settings.h"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_bool_option.hpp"
#include "helpers/teleportHelpers.h"

namespace ui::submenus
{
	void teleport_options_submenu::on_init()
	{
		this->add(toggle_option("Force Teleport", "Forcefully teleport even when ground collision is not found", &teleport::force_teleport_when_no_collision_found));
		this->add(toggle_option("Interior Collision", "Check for interior collision at coordinates and teleport inside the interior", &teleport::check_for_interior_collision));
		this->add(vector_bool_option("Teleport Animation", "Use animations when teleporting to a waypoint or objective", &teleport::using_teleport_animation, &teleport::teleport_animation, &teleport::teleport_animation_names ));
	}
}
