#include "byte_patch_manager.hpp"
#include "memory/byte_patch.hpp"
#include "memory/pointers.h"
#include "util/patches.hpp"

extern "C" void sound_overload_detour();
uint64_t g_sound_overload_ret_addr;

static void init()
{
	// Restore max wanted level after menu unload
	police::max_wanted_level = memory::byte_patch::make(g_pointers->g_max_wanted_level.add(5).rip().as<uint32_t*>(), 0).get();
	police::max_wanted_level_2 = memory::byte_patch::make(g_pointers->g_max_wanted_level.add(14).rip().as<uint32_t*>(), 0).get();

	// Patch World Model Spawn Bypass
	std::array<uint8_t, 24> world_spawn_patch;
	std::ranges::fill(world_spawn_patch, 0x90);
	world_model::world_model_spawn_bypass = memory::byte_patch::make(g_pointers->g_world_model_spawn_bypass, world_spawn_patch).get();

	// Patch blocked explosions
	explosion_anti_cheat_bypass::can_blame_others = memory::byte_patch::make(g_pointers->g_blame_explode.as<uint16_t*>(), 0xE990).get();
	explosion_anti_cheat_bypass::can_use_blocked_explosions = memory::byte_patch::make(g_pointers->g_explosion_patch.sub(12).as<uint16_t*>(), 0x9090).get();

	// Skip matchmaking session validity checks
	memory::byte_patch::make(g_pointers->g_is_matchmaking_session_valid.as<void*>(), std::to_array({ 0xB0, 0x01, 0xC3 }))->apply(); // has no observable side effects

	// Bypass net array buffer cache when enabled
	broadcast_net_array::m_patch = memory::byte_patch::make(g_pointers->g_broadcast_patch.as<uint8_t*>(), 0xEB).get();

	// Disable cheat activated net event when creator warping
	memory::byte_patch::make(g_pointers->g_creator_warp_cheat_triggered_patch.as<uint8_t*>(), 0xEB)->apply();

	if (GetModuleHandleA("ScriptHookV.dll") != nullptr)
	{
		/*memory::byte_patch::make(g_pointers->g_onlinecheck.as<void*>(), std::to_array({ 0xEB }))->apply();*/
		/*memory::byte_patch::make(g_pointers->g_poolstuff.as<void*>(), std::vector<uint8_t>({ 34, 0x90 }))->apply();
		memory::byte_patch::make(g_pointers->g_poolstuff.add(34).as<void*>(), std::to_array({ 0xEB }))->apply();*/
		memory::byte_patch::make(g_pointers->g_morepoolstuff.as<void*>(), std::vector<uint8_t>({ 10, 0x90 }))->apply();
	}

	// Crash Trigger
	memory::byte_patch::make(g_pointers->g_crash_trigger.add(4).as<uint8_t*>(), 0x00)->apply();

	// Script VM patches
	memory::byte_patch::make(g_pointers->g_script_vm_patch_1.add(2).as<uint32_t*>(), 0xc9310272)->apply();
	memory::byte_patch::make(g_pointers->g_script_vm_patch_1.add(6).as<uint16_t*>(), 0x9090)->apply();

	memory::byte_patch::make(g_pointers->g_script_vm_patch_2.add(2).as<uint32_t*>(), 0xc9310272)->apply();
	memory::byte_patch::make(g_pointers->g_script_vm_patch_2.add(6).as<uint16_t*>(), 0x9090)->apply();

	memory::byte_patch::make(g_pointers->g_script_vm_patch_3.add(2).as<uint32_t*>(), 0xd2310272)->apply();
	memory::byte_patch::make(g_pointers->g_script_vm_patch_3.add(6).as<uint16_t*>(), 0x9090)->apply();

	memory::byte_patch::make(g_pointers->g_script_vm_patch_4.add(2).as<uint32_t*>(), 0xd2310272)->apply();
	memory::byte_patch::make(g_pointers->g_script_vm_patch_4.add(6).as<uint16_t*>(), 0x9090)->apply();

	memory::byte_patch::make(g_pointers->g_script_vm_patch_5.add(2).as<uint32_t*>(), 0xd2310272)->apply();
	memory::byte_patch::make(g_pointers->g_script_vm_patch_5.add(6).as<uint16_t*>(), 0x9090)->apply();

	memory::byte_patch::make(g_pointers->g_script_vm_patch_6.add(2).as<uint32_t*>(), 0xd2310272)->apply();
	memory::byte_patch::make(g_pointers->g_script_vm_patch_6.add(6).as<uint16_t*>(), 0x9090)->apply();

	// Patch script network check
	memory::byte_patch::make(g_pointers->g_model_spawn_bypass, std::vector{ 0x90, 0x90 })->apply(); // this is no longer integrity checked

	// Prevent the game from crashing when flooded with outgoing events
	memory::byte_patch::make(g_pointers->g_free_event_error, std::vector{ 0x90, 0x90, 0x90, 0x90, 0x90 })->apply();

	// Always send the special ability event
	memory::byte_patch::make(g_pointers->g_activate_special_ability_patch, std::to_array({ 0xB0, 0x01, 0xC3 }))->apply();

	// Patch the report myself sender
	memory::byte_patch::make(g_pointers->g_report_myself_sender, std::vector{0xC3})->apply();

	// Patch BattlEye network bail
	memory::byte_patch::make(g_pointers->g_be_network_bail_patch, std::to_array({0xC3}))->apply();
}

byte_patch_manager::byte_patch_manager()
{
	init();
}

byte_patch_manager::~byte_patch_manager()
{
	memory::byte_patch::restore_all();
}
