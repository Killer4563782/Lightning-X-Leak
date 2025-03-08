#pragma once
#include "commands/types/bool_command.hpp"
#include "commands/types/looped_command.hpp"
#include "memory/pointers.h"
#include "services/script_patcher/script_patcher_service.hpp"

namespace sp_cmd
{
	template <typename T>
	static T* get_command(const char* str)
	{
		return dynamic_cast<T*>(commands::command::find(str));
	}

	static auto operator ""_BC(const char* command, uint64_t)
	{
		return get_command<commands::bool_command>(command);
	}

	static auto operator ""_LC(const char* command, uint64_t)
	{
		return get_command<commands::looped_command>(command);
	}
}

inline void register_script_patches()
{
	using namespace sp_cmd;

	g_script_patcher_service->add_patch({ "freemode"_joaat, "RP", "2D 01 08 00 ? 38 00 5D ? ? ? 2A 06", 5, {0x71, 0x2E, 0x01, 0x01}, &"reveal_players"_BC->m_toggle });										// Reveal Players
	g_script_patcher_service->add_patch({ "freemode"_joaat, "SHK", "2D 01 04 00 ? 2C ? ? ? 5D ? ? ? 71 57 ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr });												// Script Host Kick
	g_script_patcher_service->add_patch({ "freemode"_joaat, "ESKP", "5D ? ? ? 76 57 ? ? 5D ? ? ? 76", 0, {0x2E, 0x00, 0x00}, nullptr });															// End Session Kick Protection
	g_script_patcher_service->add_patch({ "freemode"_joaat, "DDB", "2D 01 09 00 00 5D ? ? ? 56 ? ? 3A", 5, {0x2E, 0x01, 0x00}, nullptr });														// Disable Death Barrier
	g_script_patcher_service->add_patch({ "freemode"_joaat, "LI", "71 2E ? ? 55 ? ? 61 ? ? ? 47 ? ? 63", 0, {0x72}, nullptr });																	// Load Island even if Stranded Animal IPL choice is not set
	g_script_patcher_service->add_patch({ "freemode"_joaat, "DPLB", "2D 00 07 00 00 7B", 5, {0x2E, 0x00, 0x00}, nullptr });																		// Disable Population Load Balancing
	g_script_patcher_service->add_patch({ "freemode"_joaat, "BM", "2D 02 08 00 00 38 01 56", 5, {0x2E, 0x02, 0x00}, &"block_muggers"_BC->m_toggle });														// Block Muggers
	g_script_patcher_service->add_patch({ "freemode"_joaat, "BCR", "2D 00 CF 00 00", 5, {0x2E, 0x00, 0x00}, &"block_ceo_raids"_BC->m_toggle });																// Block CEO Raids
	g_script_patcher_service->add_patch({ "shop_controller"_joaat, "MPVISPB", "2D 01 04 00 00 2C ? ? ? 56 ? ? 71", 5, {0x71, 0x2E, 0x01, 0x01}, nullptr });										// MP Vehicles in SP Bypass
	g_script_patcher_service->add_patch({ "shop_controller"_joaat, "GIDB", "2D 01 03 00 00 5D ? ? ? 06 56 ? ? 2E ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr });										// Godmode/Invisibility Detection Bypass
	g_script_patcher_service->add_patch({ "freemode"_joaat, "GM", "5D ? ? ? 56 ? ? 72 39 05 38 04 2C ? ? ? 58", 0, {0x2B, 0x2B, 0x2B, 0x00, 0x55}, &"godmode"_LC->m_toggle });								// Godmode
	g_script_patcher_service->add_patch({ "freemode"_joaat, "RP2", "2D 01 03 00 00 38 00 71 72 5D ? ? ? 06 56 ? ? 71 2E ? ? 2C ? ? ? 71", 5, {0x72, 0x2E, 0x01, 0x01}, &"reveal_players"_BC->m_toggle });	// Reveal Players 2
	g_script_patcher_service->add_patch({ "carmod_shop"_joaat, "AAV2", "2D 03 07 00 00 71 38 02", 5, {0x72, 0x2E, 0x03, 0x01}, nullptr });														// Allow All Vehicles 2

	for (const auto& [m_program, m_Pad1, m_hash] : *g_pointers->g_scr_program_table)
	{
		if (m_program)
		{
			g_script_patcher_service->on_script_load(m_program);
		}
	}
}

inline void unregister_script_patches()
{
	g_script_patcher_service->disable_all();
}
