#include "commands/types/looped_command.hpp"
#include "helpers/entityHelpers.h"

namespace commands
{
	class show_area final : looped_command 
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			auto my_pos = self::position;
			for (auto entity : entity::get_entities(true, true, true, true, true))
			{
				auto coords = ENTITY::GET_ENTITY_COORDS(entity, TRUE);
				Vector3 color = { 255.f, 255.f, 255.f };
				switch (ENTITY::GET_ENTITY_TYPE(entity))
				{
					case 1:		// Ped
					{
						color = { 255.f, 0.f, 0.f };
					}
					break;
					case 2:		// Vehicle
					{
						color = { 0.f, 0.f, 255.f };
					}
					break;
					case 3:		// Object & Pickups
					{
						color = { 0.f, 255.f, 0.f };
					}
					break;
				}

				if (SYSTEM::VDIST(my_pos.x, my_pos.y, my_pos.z, coords.x, coords.y, coords.z) <= 100.f)
				{
					GRAPHICS::DRAW_MARKER(
						2, coords.x, coords.y, coords.z + 2.f,	// Type + Pos 
						0.f, 0.f, 0.f, 0.f, 180.f, 0.f,			// Dir + Rot 
						1.f, 1.f, 1.f,							// Scale 
						static_cast<int>(color.x),				// Red 
						static_cast<int>(color.y),				// Green
						static_cast<int>(color.z), 50,			// Blue + Alpha
						0, 1, 1, 0, nullptr, nullptr, 0			// Misc
					);
				}
			}
		}
	};

	show_area g_show_area("Show Area", "show_area", "Draw a marker above entities.\nPeds = Red\nVehicles = Blue\nObjects = Green\nEverything else is White");
}
