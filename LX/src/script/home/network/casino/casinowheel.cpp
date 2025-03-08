#include <util/script_local.hpp>
#include <gui/types/function_option.hpp>
#include <rage/util.h>
#include "casinowheel.h"

namespace ui::submenus
{
	void casinowheel_submenu::on_init()
	{
		for (const auto& [prize, name] : tables::casinowheel)
		{
			this->add(function_option(name, name, [&]
			{
				g_fiber_pool->queue_job([&]
				{
					auto wheel = find_script_thread(rage::joaat("casino_lucky_wheel"));
					*scr_locals::script_local::script_local(wheel->m_stack, 298).at(14).as<int*>() = prize;
					*scr_locals::script_local::script_local(wheel->m_stack, 298).at(45).as<int*>() = 11;
				});
			}));
		}
	}
	casinowheel_submenu g_casinowheel_submenu("Casino wheel", "Well if you are here then you want to rig the wheel");
}