#include "boxesp_sub.h"

#include "visuals/visuals.h"
#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"

using namespace visuals::boxesp;

namespace ui::submenus
{
	void boxesp_submenu::on_init()
	{
		this->add(number_option("Distance Limit", "Distance limit for all boxes", &max_distance, 7000.f, 0.f, 20.f));
		this->add(toggle_option("Include Self", "Draw the box on the own ped", &include_self));

		this->add(toggle_option("Player Boxes", "Draw boxes on players", &player_boxes));
		this->add(color_option("Player Color", "Color of the box when the target is a player", &player_color, true));

		this->add(toggle_option("Ped Boxes", "Draw boxes on only human peds", &ped_boxes));
		this->add(color_option("Ped Color", "Color of the box when the target is a pedestrian", &ped_color, true));
	}
}
