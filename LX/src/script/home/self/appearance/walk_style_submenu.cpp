#include "walk_style_submenu.h"
#include "gui/types/function_option.hpp"
#include "fiber/script.hpp"


namespace ui::submenus
{
	void walk_style_submenu::on_init()
	{
        for (const auto& [fst, snd] : g_walk_map)
        {
            this->add(function_option(fst, {}, [&]
            {
                g_fiber_pool->queue_job([&]
                {
                	for (int i = 0; i < 100 && !STREAMING::HAS_ANIM_SET_LOADED(snd); i++)
                	{
                        if (STREAMING::HAS_ANIM_DICT_LOADED(snd))
                        {
	                        break;
                        }
                		STREAMING::REQUEST_ANIM_SET(snd);
                		script::get_current()->yield();
                	}
                    PED::SET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), snd, 1.0f);
                }); 
            })); 
        }
	}
}