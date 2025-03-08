#include "commands/types/looped_command.hpp"
#include "services/players/player_manager.hpp"
#include <util/util.h>
#include "commands/math.h"

namespace commands
{
	class rp_drop final : looped_command {
		using looped_command::looped_command;

		void on_tick() override
		{
			if (!g_player_manager->get_selected() || !g_player_manager->get_selected()->is_valid())
				return disable();

			const auto player_position = g_player_manager->get_selected()->get_pos();
			for (const auto& hash : util::rp_model_list)
			{
				if (util::natives::request_model(hash))
				{
					OBJECT::CREATE_AMBIENT_PICKUP("pickup_portable_crate_unfixed_incar_small"_joaat, player_position->x, player_position->y, player_position->z + 1.f, 0, static_cast<int>(math::random(0.f, 50.f)), hash, false, false);
				}
			}
			script::get_current()->yield(69ms);
		}

		void on_disable() override
		{
			for (const auto& hash : util::rp_model_list)
			{
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
			}
		}
	};

	rp_drop g_rp_drop("Rp Drop", "rp_drop", "Drops RP figures on player");
}
