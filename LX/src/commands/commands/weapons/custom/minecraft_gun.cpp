#include "commands/types/looped_command.hpp"
#include "util/patches.hpp"

namespace commands
{
	class minecraft_gun final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			Vector3 bullet_coord{};
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(self::ped, &bullet_coord))
			{
				const auto impact_object = world_model::spawn("prop_mb_sandblock_01"_joaat, bullet_coord);
				ENTITY::FREEZE_ENTITY_POSITION(impact_object, TRUE);
			}
		}
	};

	minecraft_gun g_minecraft_gun("Minecraft Gun", "minecraft_gun", {});
}
