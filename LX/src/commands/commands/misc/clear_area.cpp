#include "commands/types/command.hpp"
#include "helpers/entityHelpers.h"

namespace commands
{
	class clear_area final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			g_fiber_pool->queue_job([&]
			{
				const auto coords = ENTITY::GET_ENTITY_COORDS(self::ped, TRUE);
				for (auto entity : entity::get_entities(true, true, true, true, false))
				{
					constexpr float radius = 100.f;
					if (const auto entity_coords = ENTITY::GET_ENTITY_COORDS(entity, TRUE); SYSTEM::VDIST(coords.x, coords.y, coords.z, entity_coords.x, entity_coords.y, entity_coords.z) <= radius)
					{
						if (PED::IS_PED_A_PLAYER(entity))
						{
							continue;
						}
						entity::delete_selected_entity(entity, true);
					}
				}
			});
		}
	};

	clear_area g_clear_area("Clear Area", "clear_area", "Clears all peds, vehicles and objects around you");
}
