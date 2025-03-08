#include "native_hooks.hpp"

#include "all_scripts.hpp"
#include "am_launcher.hpp"
#include "carmod_shop.hpp"
#include "creator.hpp"
#include "freemode.hpp"
#include "main_transition.h"
#include "network_session_host.hpp"
#include "shop_controller.hpp"

native_hook::native_hook(rage::scrProgram* program, const std::unordered_map<NativeIndex, rage::scrNativeHandler>& native_replacements)
{
	hook_instance(program, native_replacements);
}

native_hook::~native_hook()
{
	if (m_handler_hook)
	{
		m_handler_hook->disable();
		m_handler_hook.reset();
	}

	if (m_vmt_hook)
	{
		m_vmt_hook->disable();
		m_vmt_hook.reset();
	}
}

void native_hook::hook_instance(rage::scrProgram* program, const std::unordered_map<NativeIndex, rage::scrNativeHandler>& native_replacements)
{
	m_program = program;
	m_vmt_hook = std::make_unique<vmt_hook>(m_program, 9);
	m_vmt_hook->hook(6, &scrprogram_dtor);
	m_vmt_hook->enable();

	m_handler_hook = std::make_unique<vmt_hook>(&m_program->m_native_entrypoints, m_program->m_native_count);
	m_handler_hook->enable();

	std::unordered_map<rage::scrNativeHandler, rage::scrNativeHandler> handler_replacements;

	for (auto& [replacement_index, replacement_handler] : native_replacements)
	{
		auto og_handler = native_invoker::get_handlers()[static_cast<int>(replacement_index)];
		handler_replacements[og_handler] = replacement_handler;
	}

	for (uint32_t i = 0; i < m_program->m_native_count; i++)
	{
		if (auto it = handler_replacements.find(program->m_native_entrypoints[i]); it != handler_replacements.end())
		{
			m_handler_hook->hook(i, it->second);
		}
	}
}

void native_hook::scrprogram_dtor(rage::scrProgram* this_, char free_memory)
{
	if (const auto it = g_native_hooks->m_native_hooks.find(this_); it != g_native_hooks->m_native_hooks.end())
	{
		auto og_func = it->second->m_vmt_hook->get_original<decltype(&native_hook::scrprogram_dtor)>(6);
		it->second->m_vmt_hook->disable();
		it->second->m_vmt_hook.reset();
		it->second->m_handler_hook->disable();
		it->second->m_handler_hook.reset();
		g_native_hooks->m_native_hooks.erase(it);
		og_func(this_, free_memory);
	}
	else
	{
		LOG_DEBUG("Cannot find native hook for program");
	}
}

constexpr auto ALL_SCRIPT_HASH = "ALL_SCRIPTS"_joaat;

native_hooks::native_hooks()
{
	add_native_detour(NativeIndex::NETWORK_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT, all_scripts::NETWORK_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT);
	add_native_detour(NativeIndex::NETWORK_TRY_TO_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT, all_scripts::NETWORK_TRY_TO_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT);
	add_native_detour(NativeIndex::SET_CURRENT_PED_WEAPON, all_scripts::SET_CURRENT_PED_WEAPON);
	add_native_detour(NativeIndex::DISABLE_CONTROL_ACTION, all_scripts::DISABLE_CONTROL_ACTION);
	add_native_detour(NativeIndex::HUD_FORCE_WEAPON_WHEEL, all_scripts::HUD_FORCE_WEAPON_WHEEL);
	add_native_detour(NativeIndex::NETWORK_CASINO_CAN_BET, all_scripts::RETURN_TRUE); // bypass casino country restrictions
	add_native_detour(NativeIndex::SET_ENTITY_HEALTH, all_scripts::SET_ENTITY_HEALTH);
	add_native_detour(NativeIndex::APPLY_DAMAGE_TO_PED, all_scripts::APPLY_DAMAGE_TO_PED);
	add_native_detour(NativeIndex::REGISTER_SCRIPT_VARIABLE, all_scripts::DO_NOTHING);
	add_native_detour(NativeIndex::UNREGISTER_SCRIPT_VARIABLE, all_scripts::DO_NOTHING);
	add_native_detour(NativeIndex::FORCE_CHECK_SCRIPT_VARIABLES, all_scripts::DO_NOTHING);
	add_native_detour(NativeIndex::_GET_BATTLEYE_INIT_STATE, all_scripts::RETURN_FALSE);
	add_native_detour(NativeIndex::GET_EVER_HAD_BAD_PACK_ORDER, all_scripts::RETURN_FALSE); // bypass DLC checks

	add_native_detour("shop_controller"_joaat, NativeIndex::IS_PED_SHOOTING, all_scripts::RETURN_FALSE); // prevent exploit reports
	add_native_detour("shop_controller"_joaat, NativeIndex::SET_WARNING_MESSAGE_WITH_HEADER, shop_controller::SET_WARNING_MESSAGE_WITH_HEADER);

	add_native_detour("carmod_shop"_joaat, NativeIndex::STAT_GET_INT, carmod_shop::STAT_GET_INT);
	add_native_detour("carmod_shop"_joaat, NativeIndex::STAT_SET_INT, carmod_shop::STAT_SET_INT);

	add_native_detour("freemode"_joaat, NativeIndex::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH, freemode::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH);
	add_native_detour("freemode"_joaat, NativeIndex::IS_PLAYER_PLAYING, freemode::IS_PLAYER_PLAYING);
	add_native_detour("freemode"_joaat, NativeIndex::SET_ENTITY_VISIBLE, freemode::SET_ENTITY_VISIBLE);
	add_native_detour("freemode"_joaat, NativeIndex::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA, freemode::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA);

	add_native_detour("fmmc_launcher"_joaat, NativeIndex::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA, freemode::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA);
	add_native_detour("maintransition"_joaat, NativeIndex::NETWORK_SESSION_HOST, network::NETWORK_SESSION_HOST);

	add_native_detour("am_launcher"_joaat, NativeIndex::START_NEW_SCRIPT_WITH_ARGS, am_launcher::START_NEW_SCRIPT_WITH_ARGS);
	add_native_detour("am_launcher"_joaat, NativeIndex::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA, freemode::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA);

	add_native_detour("fm_race_creator"_joaat, NativeIndex::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH, creator::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH);
	add_native_detour("fm_capture_creator"_joaat, NativeIndex::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH, creator::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH);
	add_native_detour("fm_deathmatch_creator"_joaat, NativeIndex::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH, creator::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH);
	add_native_detour("fm_lts_creator"_joaat, NativeIndex::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH, creator::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH);

	add_native_detour("fm_race_creator"_joaat, NativeIndex::GET_ENTITY_MODEL, creator::GET_ENTITY_MODEL);
	add_native_detour("fm_capture_creator"_joaat, NativeIndex::GET_ENTITY_MODEL, creator::GET_ENTITY_MODEL);
	add_native_detour("fm_deathmatch_creator"_joaat, NativeIndex::GET_ENTITY_MODEL, creator::GET_ENTITY_MODEL);
	add_native_detour("fm_lts_creator"_joaat, NativeIndex::GET_ENTITY_MODEL, creator::GET_ENTITY_MODEL);

	add_native_detour("maintransition"_joaat, NativeIndex::NETWORK_CAN_BAIL, main_transition::NETWORK_CAN_BAIL);
	add_native_detour("maintransition"_joaat, NativeIndex::NETWORK_BAIL, main_transition::NETWORK_BAIL);
	add_native_detour("freemode"_joaat, NativeIndex::NETWORK_BAIL, main_transition::NETWORK_BAIL);

	// TODO: is this safe?
	add_native_detour("arena_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("armory_aircraft_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("base_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("business_hub_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("car_meet_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("carmod_shop"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("fixer_hq_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("hacker_truck_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("hangar_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("juggalo_hideout_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("personal_carmod_shop"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("tuner_property_carmod"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("clothes_shop_mp"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("gunclub_shop"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("hairdo_shop_mp"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.
	add_native_detour("tattoo_shop"_joaat, NativeIndex::FORCE_PED_AI_AND_ANIMATION_UPDATE, all_scripts::DO_NOTHING); //Fix jittering weapons.

	// SEAMLESS JOINING
	add_native_detour(NativeIndex::SC_TRANSITION_NEWS_SHOW, main_transition::SC_TRANSITION_NEWS_SHOW);       // Stops news.
	add_native_detour(NativeIndex::SC_TRANSITION_NEWS_SHOW_TIMED, main_transition::SC_TRANSITION_NEWS_SHOW_TIMED); // Stops news.
	add_native_detour("maintransition"_joaat, NativeIndex::CLEAR_PED_TASKS_IMMEDIATELY, main_transition::CLEAR_PED_TASKS_IMMEDIATELY);
	add_native_detour("maintransition"_joaat, NativeIndex::IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED, main_transition::IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED); // This hook lets you stop player-switch in "Pre-HUD Checks"
	add_native_detour("maintransition"_joaat, NativeIndex::SET_FOCUS_ENTITY, main_transition::SET_FOCUS_ENTITY); // Prevets map from unloading.
	add_native_detour("maintransition"_joaat, NativeIndex::ACTIVATE_FRONTEND_MENU, main_transition::ACTIVATE_FRONTEND_MENU); // Let's you controll your ped when going sp to mp.
	add_native_detour("maintransition"_joaat, NativeIndex::RESTART_FRONTEND_MENU, main_transition::RESTART_FRONTEND_MENU); // Let's you controll your ped when going sp to mp.
	add_native_detour("maintransition"_joaat, NativeIndex::TOGGLE_PAUSED_RENDERPHASES, main_transition::TOGGLE_PAUSED_RENDERPHASES); // Prevents the game from freezing your screen.
	add_native_detour("maintransition"_joaat, NativeIndex::SET_ENTITY_VISIBLE, main_transition::SET_ENTITY_VISIBLE); // Makes you visible.
	add_native_detour("maintransition"_joaat, NativeIndex::SET_PED_COORDS_KEEP_VEHICLE, main_transition::SET_PED_COORDS_KEEP_VEHICLE); // Prevents the game from teleporting you.
	add_native_detour("maintransition"_joaat, NativeIndex::SET_ENTITY_COLLISION, main_transition::SET_ENTITY_COLLISION); // Prevents you from falling.
	add_native_detour("maintransition"_joaat, NativeIndex::SET_PLAYER_CONTROL, main_transition::SET_PLAYER_CONTROL); // Allows controll in session switch.
	add_native_detour("maintransition"_joaat, NativeIndex::FREEZE_ENTITY_POSITION, main_transition::FREEZE_ENTITY_POSITION); // Allows controll in session switch.
	add_native_detour("maintransition"_joaat, NativeIndex::NETWORK_RESURRECT_LOCAL_PLAYER, main_transition::NETWORK_RESURRECT_LOCAL_PLAYER); // Prevents player from teleporting after switch.
	add_native_detour("maintransition"_joaat, NativeIndex::GET_EVER_HAD_BAD_PACK_ORDER, main_transition::GET_EVER_HAD_BAD_PACK_ORDER); // Prevent weird reloading when using custom dlcs.

	for (const auto& [m_program, m_Pad1, m_hash] : *g_pointers->g_scr_program_table)
		if (m_program)
			hook_program(m_program);
}

native_hooks::~native_hooks()
{
	m_native_hooks.clear();
}

void native_hooks::add_native_detour(NativeIndex index, rage::scrNativeHandler detour)
{
	add_native_detour(ALL_SCRIPT_HASH, index, detour);
}

void native_hooks::add_native_detour(rage::joaat_t script_hash, NativeIndex index, rage::scrNativeHandler detour)
{
	if (const auto& it = m_native_registrations.find(script_hash); it != m_native_registrations.end())
	{
		it->second.emplace_back(index, detour);
		return;
	}

	m_native_registrations.emplace(script_hash, std::vector<native_detour>({ {index, detour} }));
}

void native_hooks::hook_program(rage::scrProgram* program)
{
	std::unordered_map<NativeIndex, rage::scrNativeHandler> native_replacements;
	const auto script_hash = program->m_name_hash;

	// Functions that need to be detoured for all scripts
	if (const auto& pair = m_native_registrations.find(ALL_SCRIPT_HASH); pair != m_native_registrations.end())
		for (const auto& native_hook_reg : pair->second)
			native_replacements.insert(native_hook_reg);

	// Functions that only need to be detoured for a specific script
	if (const auto& pair = m_native_registrations.find(script_hash); pair != m_native_registrations.end())
		for (const auto& native_hook_reg : pair->second)
			native_replacements.insert(native_hook_reg);

	if (!native_replacements.empty())
	{
		m_native_hooks.emplace(program, std::make_unique<native_hook>(program, native_replacements));
	}
}
