#include "hooking/hooking.hpp"
#include "services/reactions/reactions.h"

constexpr uint32_t valid_parachute_models[] = {
	"p_parachute_s"_joaat,
	"vw_p_para_bag_vine_s"_joaat,
	"reh_p_para_bag_reh_s_01a"_joaat,
	"xm_prop_x17_scuba_tank"_joaat,
	"lts_p_para_bag_pilot2_s"_joaat,
	"lts_p_para_bag_lts_s"_joaat,
	"p_para_bag_tr_s_01a"_joaat,
	"p_para_bag_xmas_s"_joaat,
};

bool is_valid_parachute_model(const rage::joaat_t hash)
{
	for (auto& model : valid_parachute_models)
		if (model == hash)
			return true;

	return false;
}

void hooks::serialize_take_off_ped_variation_task(ClonedTakeOffPedVariationInfo* info, rage::CSyncDataBase* serializer)
{
	if (!is_valid_parachute_model(info->m_prop_hash))
	{
		if (crash.process(g_syncing_player, "Crash V2"))
		{
			info->m_prop_hash = 0;
		}
		return;
	}

	return g_hooking->get_original<serialize_take_off_ped_variation_task>()(info, serializer);
}
