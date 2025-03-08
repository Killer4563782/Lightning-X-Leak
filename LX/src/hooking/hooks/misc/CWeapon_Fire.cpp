#include "commands/manager.hpp"
#include "hooking/hooking.hpp"

bool hooks::c_weapon_fire(__int64 _this, __int64 params)
{
	if ("silent_aimbot"_BC->m_toggle)
	{
		if (Entity handle; PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(self::player, &handle) && ENTITY::IS_ENTITY_A_PED(handle) && !PED::IS_PED_DEAD_OR_DYING(handle, false))
		{
			if (auto entity = g_pointers->g_handle_to_pointer(handle); entity && entity->m_entity_type == 4)
			{
				rage::fvector3* end = *reinterpret_cast<rage::fvector3**>(params + 0x38);
				rage::fvector3 pos = reinterpret_cast<CPed*>(entity)->get_bone_coords(ePedBoneType::HEAD);

				end->x = pos.x;
				end->y = pos.y;
				end->z = pos.z;
			}
		}
	}

	return g_hooking->get_original<c_weapon_fire>()(_this, params);
}
