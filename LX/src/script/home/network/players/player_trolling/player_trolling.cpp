#include "player_trolling.h"

#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "helpers/teleportHelpers.h"
#include "util/patches.hpp"

namespace ui::submenus
{
	void player_trolling_submenu::on_init()
	{
		/*this->add(function_option("Delete Ped Crash", {}, [this]
		{
			g_fiber_pool->queue_job([]
			{
				auto convert = g_pointers->g_pointerToHandle(g_player_manager->get_selected()->get_ped());
				if (!convert)
				{
					return notification(Error, "Failed to convert Player to handle");
				}
				if (!ENTITY::IS_ENTITY_A_MISSION_ENTITY(convert))
				{
					ENTITY::SET_ENTITY_AS_MISSION_ENTITY(convert, TRUE, TRUE);
				}
				ENTITY::DELETE_ENTITY(&convert);
			});
		}));*/
		this->add(function_option("Cage", {}, [this]
		{
			g_fiber_pool->queue_job([]
			{
				if (auto ped = g_player_manager->get_selected()->get_ped())
				{
					auto object = world_model::spawn("prop_gold_cont_01"_joaat, { 0.f, 0.f, 1000.f });
					auto pos = ped->get_position();
					ENTITY::FREEZE_ENTITY_POSITION(object, TRUE);
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(object, pos->x, pos->y, pos->z, FALSE, FALSE, FALSE);
				}
			});
		}));
		this->add(function_option("Anonymous Explode", {}, [this]
		{
			g_fiber_pool->queue_job([]
			{
				if (auto pos = g_player_manager->get_selected()->get_pos())
				{
					explosion_anti_cheat_bypass::explode(
						PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id()),
						{ pos->x, pos->y, pos->z },
						ROCKET,
						9999.f,
						true,
						false
					);
				}
			});
		}));
		this->add(function_option("Explode", {}, [this]
		{
			g_fiber_pool->queue_job([]
			{
				if (auto pos = g_player_manager->get_selected()->get_pos())
				{
					explosion_anti_cheat_bypass::explode(
						self::ped,
						{ pos->x, pos->y, pos->z },
						ROCKET,
						9999.f,
						true,
						false
					);
				}
			});
		}));
		this->add(toggle_option("remove_godmode"_LC));
		this->add(function_option("Electrocute", {}, [this]
		{
			g_fiber_pool->queue_job([]
			{
				if (const auto pos = g_player_manager->get_selected()->get_pos())
				{
					constexpr auto hash = "weapon_stungun"_joaat;
					const auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id());
					if (!util::natives::request_weapon_asset(hash))
						return;

					const auto bone = PED::GET_PED_BONE_COORDS(ped, IK_Head, 0, 0, 0);
					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos->x, pos->y, pos->z, bone.x, bone.y, bone.z, 0, TRUE, hash, self::ped, TRUE, FALSE, -1.f);
				}
			});
		}));
		this->add(function_option("Kill", {}, [this]
		{
			g_fiber_pool->queue_job([]
			{
				if (const auto pos = g_player_manager->get_selected()->get_pos())
				{
					constexpr auto hash = "weapon_heavysniper_mk2"_joaat;
					const auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id());
					if (!util::natives::request_weapon_asset(hash))
						return;

					const auto bone = PED::GET_PED_BONE_COORDS(ped, IK_Head, 0, 0, 0);
					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos->x, pos->y, pos->z, bone.x, bone.y, bone.z, 500, TRUE, hash, self::ped, TRUE, FALSE, -1.f);

					/*explosion_anti_cheat_bypass::explode(
						self::ped,
						pos,
						BULLET,
						9999.f,
						true,
						false
					);*/
				}
			});
		}));
	}
}
