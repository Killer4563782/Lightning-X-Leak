#include "visuals.h"
#include "commands/math.h"

#include "helpers/entityHelpers.h"

namespace ui::visuals::boxesp
{
	void draw_boxes()
	{
		if (ImGui::Begin("##BoxESP", nullptr, 935))
		{
			for (auto& entity : entity::get_entities(false, ped_boxes || player_boxes, false, false, false, include_self))
			{
				if (!ENTITY::IS_ENTITY_A_PED(entity))
					continue;

				if (auto pos = ENTITY::GET_ENTITY_COORDS(entity, TRUE); math::distance_between_vectors(self::position, pos) > max_distance)
					continue;

				if (PED::GET_PED_TYPE(entity) == PED_TYPE_ANIMAL)
					continue;

				if (entity == PLAYER::PLAYER_PED_ID() && !include_self)
					continue;

				if (PED::IS_PED_A_PLAYER(entity))
				{
					if (!player_boxes)
						continue;
				}
				else if (!ped_boxes && player_boxes && entity != PLAYER::PLAYER_PED_ID()) // not sure if second-third check is needed for story mode or nah
					continue;

				ImVec2 head_pos, bottom_pos;
				if (!get_screen_bone_pos(entity, SKEL_Head, &head_pos, { 1.f, 0.f, 0.f }) || !get_screen_bone_pos(entity, SKEL_Head, &bottom_pos, { -1.1f, 0.f, 0.f }))
					continue;

				float height = std::abs(head_pos.y - bottom_pos.y);
				float width = height / 4;

				drawing::rectangle_not_filled({ head_pos.x - width / 2, head_pos.y }, { width, height }, PED::IS_PED_A_PLAYER(entity) ? player_color : ped_color, 1.f, true);
			}
		}
		ImGui::End();
	}
}
