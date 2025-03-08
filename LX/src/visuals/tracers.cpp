#include "visuals.h"

#include "helpers/entityHelpers.h"

namespace ui::visuals::tracers
{
	void draw_tracers()
	{
		if (ImGui::Begin("##Tracers", nullptr, 935))
		{
			for (auto& entity : entity::get_entities(vehicle_tracers, ped_tracers || player_tracers, prop_tracers, prop_tracers, include_self, include_self))
			{
				auto self_pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);
				auto pos = ENTITY::GET_ENTITY_COORDS(entity, TRUE);
				if (SYSTEM::VDIST(self_pos.x, self_pos.y, self_pos.z, pos.x, pos.y, pos.z) > max_distance)
					continue;

				// Peds & Players
				if (ENTITY::IS_ENTITY_A_PED(entity))
				{
					if (PED::GET_PED_TYPE(entity) == PED_TYPE_ANIMAL && only_humans)
						continue;

					if (entity == PLAYER::PLAYER_PED_ID() && !include_self)
						continue;

					if (PED::IS_PED_A_PLAYER(entity))
					{
						if (!player_tracers)
							continue;
					}
					else if (!ped_tracers && player_tracers && entity != PLAYER::PLAYER_PED_ID()) // not sure if second-third check is needed for story mode or nah
						continue;

					if (use_native_3d)
					{
						auto head_pos = get_bone_pos(entity, SKEL_Head, { 0.1f, 0.0f, 0.f });
						color col = PED::IS_PED_A_PLAYER(entity) ? player_color : ped_color;

						GRAPHICS::DRAW_LINE(self_pos.x, self_pos.y, self_pos.z, head_pos.x, head_pos.y, head_pos.z, col.r, col.g, col.b, col.a);
					}
					else if (ImVec2 head_pos; get_screen_bone_pos(entity, SKEL_Head, &head_pos, { 0.1f, 0.0f, 0.f }))
					{
						drawing::line(start_pos, head_pos, PED::IS_PED_A_PLAYER(entity) ? player_color : ped_color, 1.f, true);
					}
				}
				// Vehicle, Objects & Pickups
				else
				{
					color col = { 255, 255, 255, 255 };
					switch (ENTITY::GET_ENTITY_TYPE(entity))
					{
					case 2: // Vehicle 
					{
						col = vehicle_color;
						break;
					}

					case 3:	// Object & Pickups
					{
						col = prop_color;
						break;
					}
					}

					if (use_native_3d)
					{
						GRAPHICS::DRAW_LINE(self_pos.x, self_pos.y, self_pos.z, pos.x, pos.y, pos.z, col.r, col.g, col.b, col.a);
					}
					else if (ImVec2 screen_pos; world_to_screen(ENTITY::GET_ENTITY_COORDS(entity, TRUE), &screen_pos))
					{
						drawing::line(start_pos, screen_pos, col, 1.f, true);
					}
				}

			}
		}
		ImGui::End();
	}
}
