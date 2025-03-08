#include "visuals.h"
#include "helpers/entityHelpers.h"
#include "helpers/playerInfo.h"
#include "services/players/player_manager.hpp"

namespace ui::visuals::names
{
	void draw_names()
	{
		if (ImGui::Begin("##Names", nullptr, 935))
		{
			if (*g_pointers->g_is_session_active)
			{
				auto self = g_player_manager->get_self();
				if (!self || !self->get_ped())
					return;

				auto self_ped = self->get_ped();
				auto self_pos = self_ped->get_position();

				if (include_self)
				{
					auto entity = g_pointers->g_pointer_to_handle(self_ped);
					ImVec2 head_pos;
					if (get_screen_bone_pos(entity, SKEL_Head, &head_pos, { 0.5f, 0.f, 0.f }))
					{
						auto tags = player_info::get_player_tags(self);
						drawing::text(Arial, std::string(self->get_name()) + (show_tags && !tags.empty() ? " " + tags : ""), head_pos, 0.3f, player_color, eJustify::Center, 0.f, 0.f, true);
					}
				}

				for (auto& player : g_player_manager->players() | std::views::values)
				{
					if (!player || !player->get_ped())
						continue;

					auto player_ped = player->get_ped();
					auto player_pos = player_ped->get_position();

					if (SYSTEM::VDIST(self_pos->x, self_pos->y, self_pos->z, player_pos->x, player_pos->y, player_pos->z) > max_distance)
						continue;

					if (!player_names)
						continue;

					auto entity = g_pointers->g_pointer_to_handle(player_ped);
					ImVec2 head_pos;
					if (!get_screen_bone_pos(entity, SKEL_Head, &head_pos, { 0.5f, 0.f, 0.f }))
						continue;

					auto tags = player_info::get_player_tags(player);
					drawing::text(Arial, std::string(player->get_name()) + (show_tags && !tags.empty() ? " " + tags : ""), head_pos, 0.3f, player_color, eJustify::Center, 0.f, 0.f, true);
				}
			}
			else if (include_self)
			{
				auto entity = PLAYER::PLAYER_PED_ID();
				ImVec2 head_pos;
				if (!get_screen_bone_pos(entity, SKEL_Head, &head_pos, { 0.5f, 0.f, 0.f }))
					return;

				drawing::text(Arial, g_pointers->g_sc_info->m_sc_name, head_pos, 0.3f, player_color, eJustify::Center, 0.f, 0.f, true);
			}
		}
		ImGui::End();
	}
}
