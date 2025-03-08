#include "casino.hpp"
#include "gui/types/number_option.hpp"
#include "casinowheel.h"

namespace ui::submenus
{
	void casino_submenu::on_init()
	{
		this->add(&g_casinowheel_submenu);
	}
	casino_submenu g_casino_submenu("Casino", "Casino options");
}