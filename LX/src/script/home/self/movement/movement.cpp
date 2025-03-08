#include "movement.h"

#include "commands/manager.hpp"
#include "gui/types/number_bool_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_bool_option.hpp"

namespace ui::submenus
{
	void movement_submenu::on_init()
	{
		this->add(toggle_option("free_movement_in_interior"_LC));
		this->add(vector_bool_option("super_jump"_LVC));
		this->add(vector_bool_option("graceful_landing"_LVC));
		this->add(number_bool_option<float>("walking_speed"_LFC));
		this->add(number_bool_option<float>("swim_speed"_LFC));
	}
}
