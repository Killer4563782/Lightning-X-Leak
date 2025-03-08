#include "model_changer_submenu.h"

#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/icon_option.hpp"
#include "helpers/inputs.h"
#include "helpers/networkHelpers.h"
#include "services/entity_preview/entity_preview.h"
#include "util/util.h"

namespace ui::submenus
{
	void model_changer_submenu::on_enter()
	{
		m_current = 0;
		m_scroll_position = 0;
	}

	void model_changer_submenu::on_init()
	{
		this->add(icon_option("Input Model", "Enter model name", ICON_FA_KEYBOARD, []
		{
			get_input("Enter model name", "Enter model name", text_input::InputFlags_None, [](const std::string& input)
			{
				g_fiber_pool->queue_job([input]
				{
					network_helpers::set_player_model(rage::joaat(input));
				});
			});
		}));
		this->add(function_option("Reset model", {}, []
		{
			g_fiber_pool->queue_job([]
			{
				services::is_online() ? network_helpers::set_player_model("mp_m_freemode_01"_joaat) : network_helpers::set_player_model("player_one"_joaat);
			});
		}));
		this->add(break_option("Models"));
		for (const auto& model : g_ped_models)
		{
			this->add(function_option(model.first, {}, [model]
			{
				g_fiber_pool->queue_job([model]
				{
					network_helpers::set_player_model(model.second);
				});
			})); 
		}
	}

	void model_changer_submenu::on_tick()
	{
		if (m_current <= 2)
			return;

		entity_preview::ped::preview(g_ped_models[m_current - 3].second);
	}
}
