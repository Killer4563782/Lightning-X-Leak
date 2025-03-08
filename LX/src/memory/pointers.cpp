#include "memory/pointers.h"
#include "scanner.h"

pointers::pointers()
{
	g_hwnd = FindWindowA("grcWindow", nullptr);
	if (!g_hwnd)
	{
		g_running = false;
		return;
	}

	static pattern_batch main_batch{};
	const auto start = steady_clock::now();

	main_batch.add("LSS", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", [this](mem ptr)
	{
		g_loading_screen_state = ptr.lea().add(1).as<decltype(g_loading_screen_state)>();
	});

	main_batch.add("NS", "90 C3 CC", [this](mem ptr)
	{
		g_nullsub = ptr.as<decltype(g_nullsub)>();
	});

	main_batch.add("GTS", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 8B FA 85", [this](mem ptr)
	{
		g_gta_thread_start = ptr.as<decltype(g_gta_thread_start)>();
	});

	main_batch.add("GTK", "48 89 5C 24 08 57 48 83 EC 20 48 83 B9 18 01", [this](mem ptr)
	{
		g_gta_thread_kill = ptr.as<decltype(g_gta_thread_kill)>();
	});

	main_batch.add("ST", "45 33 F6 8B E9 85 C9 B8", [this](mem ptr)
	{
		g_script_threads = ptr.sub(4).rip().sub(8).as<decltype(g_script_threads)>();
		g_run_script_threads = ptr.sub(0x1F).as<decltype(g_run_script_threads)>();
	});

	main_batch.add("GSC", "40 B6 01 39 79 2C", [this](mem ptr)
	{
		g_get_scr_command = ptr.add(0x17).call().as<decltype(g_get_scr_command)>();
	});

	main_batch.add("FC", "8B 15 ? ? ? ? 41 FF CF", [this](mem ptr)
	{
		g_frame_count = ptr.lea().as<decltype(g_frame_count)>();
	});

	main_batch.add("HTP", "83 F9 FF 74 31 4C", [this](mem ptr)
	{
		g_handle_to_pointer = ptr.as<decltype(g_handle_to_pointer)>();
	});

	main_batch.add("IMA", "83 C8 01 48 8D 0D ? ? ? ? 41 B1 01 45 33 C0", [this](mem ptr)
	{
		*ptr.add(17).as<uint32_t*>() = 650 * 1024 * 1024;
	});

	main_batch.add("SCS", "49 63 F0 48 8B EA B9 07 00 00 00", [this](mem ptr)
	{
		g_smpa_create_stub = ptr.sub(0x29).as<decltype(g_smpa_create_stub)>();
	});

	main_batch.add("RGC", "48 89 5C 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 30 48 8B F9", [this](mem ptr)
	{
		g_read_game_config = ptr.as<decltype(g_read_game_config)>();
	});

	main_batch.add("GPS", "45 33 DB 44 8B D2 66 44 39 59 10 74 4B", [this](mem ptr)
	{
		g_get_pool_size = ptr.as<decltype(g_get_pool_size)>();
	});

	main_batch.add("CP", "40 53 48 83 EC 20 8B 44 24 50 48 83", [this](mem ptr)
	{
		g_create_pool = ptr.as<decltype(g_create_pool)>();
	});

	main_batch.add("GPI", "4C 8B D1 48 63 49 18", [this](mem ptr)
	{
		g_get_pool_item = ptr.as<decltype(g_get_pool_item)>();
	});

	main_batch.add("SBPO", "80 8F 82 01 00 00 04", [this](mem ptr)
	{
		memset(ptr.as<PVOID>(), 0x90, 7);
	});

	main_batch.add("PTH", "48 8B F9 48 83 C1 10 33 DB", [this](mem ptr)
	{
		g_pointer_to_handle = ptr.sub(0x15).as<decltype(g_pointer_to_handle)>();
	});

	main_batch.add("RBWD", "48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0 8A", [this](mem ptr)
	{
		g_read_bitbuf_dword = ptr.sub(5).as<decltype(g_read_bitbuf_dword)>();
	});

	main_batch.add("RBA", "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C", [this](mem ptr)
	{
		g_read_bitbuf_array = ptr.as<decltype(g_read_bitbuf_array)>();
	});

	main_batch.add("RBS", "48 89 5C 24 08 48 89 6C 24 18 56 57 41 56 48 83 EC 20 48 8B F2 45", [this](mem ptr)
	{
		g_read_bitbuf_string = ptr.as<decltype(g_read_bitbuf_string)>();
	});

	main_batch.add("RBB", "48 8B C4 48 89 58 08 55 56 57 48 83 EC 20 48 83 60", [this](mem ptr)
	{
		g_read_bitbuf_bool = ptr.as<decltype(g_read_bitbuf_bool)>();
	});

	main_batch.add("WBD", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 8B EA BF", [this](mem ptr)
	{
		g_write_bitbuf_dword = ptr.as<decltype(g_write_bitbuf_dword)>();
	});

	main_batch.add("WBQ", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B EA 48 8B D9 41", [this](mem ptr)
	{
		g_write_bitbuf_qword = ptr.as<decltype(g_write_bitbuf_qword)>();
	});

	main_batch.add("WBI64", "E8 ? ? ? ? 8A 53 39 48 8B CF", [this](mem ptr)
	{
		g_write_bitbuf_int64 = ptr.call().as<decltype(g_write_bitbuf_int64)>();
	});

	main_batch.add("WBI32", "E8 ? ? ? ? 8A 53 74", [this](mem ptr)
	{
		g_write_bitbuf_int32 = ptr.call().as<decltype(g_write_bitbuf_int32)>();
	});

	main_batch.add("WBB", "E8 ? ? ? ? 8A 57 39", [this](mem ptr)
	{
		g_write_bitbuf_bool = ptr.call().as<decltype(g_write_bitbuf_bool)>();
	});

	main_batch.add("WBA", "E8 ? ? ? ? 01 7E 08", [this](mem ptr)
	{
		g_write_bitbuf_array = ptr.call().as<decltype(g_write_bitbuf_array)>();
	});

	main_batch.add("CTJC", "48 89 5C 24 08 89 54 24 10 57 48 83 EC 30 0F", [this](mem ptr)
	{
		g_c_task_jump_constructor = ptr.as<decltype(g_c_task_jump_constructor)>();
	});

	main_batch.add("CTFC", "48 89 5C 24 08 89 54 24 10 57 48 83 EC 20 48 8B D9 E8", [this](mem ptr)
	{
		g_c_task_fall_constructor = ptr.as<decltype(g_c_task_fall_constructor)>();
	});

	main_batch.add("RAMS", "48 89 5C 24 08 57 48 83 EC 20 0F B6 99", [this](mem ptr)
	{
		g_run_async_module_request = ptr.as<decltype(g_run_async_module_request)>();
	});

	main_batch.add("HIE", "48 89 5C 24 08 57 48 83 EC 20 8B FA 8B D9 FF", [this](mem ptr)
	{
		g_has_interval_elapsed = ptr.as<decltype(g_has_interval_elapsed)>();
	});

	main_batch.add("DE", "48 83 EC 20 4C 8B 71 50 33 ED", [this](mem ptr)
	{
		g_dispatch_event = ptr.sub(0x19).as<decltype(g_dispatch_event)>();
	});

	main_batch.add("VM", "48 8B C4 4C 89 40 18 48 89 50 10 48 89 48 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC B8", [this](mem ptr)
	{
		g_script_vm = ptr.as<decltype(g_script_vm)>();
	});

	main_batch.add("GCD", "48 8B 15 ? ? ? ? 48 8B C2 48 0B C7", [this](mem ptr)
	{
		g_game_checksum_data = ptr.mov().as<decltype(g_game_checksum_data)>();
	});

	main_batch.add("GDH+GDM", "48 8B 0D ? ? ? ? 33 D2 E8 ? ? ? ? 45 33 C0", [this](mem ptr)
	{
		g_get_dlc_hash = ptr.add(10).rip().as<decltype(g_get_dlc_hash)>();
		g_dlc_manager = ptr.mov().as<decltype(g_dlc_manager)>();
	});

	main_batch.add("CWIM", "0F B7 15 ? ? ? ? ? 33 D2 2B D3 78 ? ? 8B 1D", [this](mem ptr)
	{
		g_weapon_info_manager = ptr.mov().sub(72).as<decltype(g_weapon_info_manager)>();
	});
	
	main_batch.add("REH", "66 41 83 F9 5B", [this](mem ptr)
	{
		g_received_event = ptr.as<decltype(g_received_event)>();
	});
	
	main_batch.add("SEA", "E8 ? ? ? ? 66 83 7B 08 5B", [this](mem ptr)
	{
		g_send_event_ack = ptr.call().as<decltype(g_send_event_ack)>();
	});
	
	main_batch.add("GNED", "53 43 52 49 50 54 5F 4E 45 54 57 4F 52 4B", [this](mem ptr)
	{
		g_get_network_event_data = *ptr.sub(0x38).as<PVOID*>();
	});
	
	main_batch.add("FRC", "8A 05 ? ? ? ? 88 83 BC 00 00 00", [this](mem ptr)
	{
		g_force_relay_connections = ptr.add(2).rip().as<bool*>();
	});

	main_batch.add("GNO", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 40 4C 8B EA", [this](mem ptr)
	{
		g_receive_clone_sync = ptr.as<decltype(g_receive_clone_sync)>();
		g_get_sync_tree_for_type = ptr.add(0x2F).call().as<decltype(g_get_sync_tree_for_type)>();
		g_get_net_object = ptr.add(0x109).call().as<decltype(g_get_net_object)>();
	});
	
	main_batch.add("RCC", "48 8B C4 66 44 89 48", [this](mem ptr)
	{
		g_receive_clone_create = ptr.as<decltype(g_receive_clone_create)>();
	});
	
	main_batch.add("RCR", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 54 41 56 41 57 48 83 EC 50 4C 8B F2 4D 8B E0", [this](mem ptr)
	{
		g_received_clone_remove = ptr.as<decltype(g_received_clone_remove)>();
	});
	
	main_batch.add("CAD", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 4C 8B F2 48 8B E9 BB", [this](mem ptr)
	{
		g_can_apply_data = ptr.as<decltype(g_can_apply_data)>();
	});
	
	main_batch.add("GEAT", "48 83 EC 28 48 8B 51 50 48 85 D2 74 04", [this](mem ptr)
	{
		g_get_entity_attached_to = ptr.as<decltype(g_get_entity_attached_to)>();
	});
	
	main_batch.add("ISA", "40 38 35 ? ? ? ? 75 0E 4C 8B C3", [this](mem ptr)
	{
		g_is_session_active = ptr.cmp().as<decltype(g_is_session_active)>();
	});
	
	main_batch.add("GLT", "48 8D 0D ? ? ? ? E8 ? ? ? ? 45 33 C9 41 B0 01", [this](mem ptr)
	{
		g_get_label_text = ptr.add(8).rip().as<decltype(g_get_label_text)>();
	});
	
	main_batch.add("API", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8A E8", [this](mem ptr)
	{
		g_assign_physical_index = ptr.as<decltype(g_assign_physical_index)>();
	});
	
	main_batch.add("SIAPE", "48 83 EC 38 F6 82", [this](mem ptr)
	{
		g_send_invite_accepted_presence_event = ptr.as<decltype(g_send_invite_accepted_presence_event)>();
	});
	
	main_batch.add("PS", "48 8D 0D ? ? ? ? 8B F8 E8 ? ? ? ? 3B F8", [this](mem ptr)
	{
		g_presecne_struct = ptr.mov().as<decltype(g_presecne_struct)>();
	});
	
	main_batch.add("JRP", "23 C3 C1 E0 0C", [this](mem ptr)
	{
		g_jmp_rbx_register = ptr.sub(1).as<decltype(g_jmp_rbx_register)>();
	});
	
	main_batch.add("GHAHBI", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 8A 81 97", [this](mem ptr)
	{
		g_get_host_array_handler_by_index = ptr.as<decltype(g_get_host_array_handler_by_index)>();
	});
	
	main_batch.add("E0MB", "E8 ? ? ? ? CC FF 15", [this](mem ptr)
	{
		g_error_message_box = ptr.call().as<decltype(g_error_message_box)>();
	});
	
	main_batch.add("GTCE0MB", "E8 ? ? ? ? 48 83 CB FF 48 8D 8D", [this](mem ptr)
	{
		g_get_title_caption_error_message_box = ptr.call().as<decltype(g_get_title_caption_error_message_box)>();
	});
	
	main_batch.add("CMTFM", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 48 8B D9 48 8B 49 18", [this](mem ptr)
	{
		g_allocate_memory_reliable = ptr.as<decltype(g_allocate_memory_reliable)>();
		g_connection_manager_try_free_memory = ptr.add(0x52).rip().as<decltype(g_connection_manager_try_free_memory)>();
	});
	
	main_batch.add("RMFQ", "E8 ? ? ? ? 0F B7 43 4C 48 8D 55 20", [this](mem ptr)
	{
		g_remove_message_from_queue = ptr.call().as<decltype(g_remove_message_from_queue)>();
		g_remove_message_from_unacked_reliables = ptr.add(0x19).rip().as<decltype(g_remove_message_from_unacked_reliables)>();
	});
	
	main_batch.add("GSCFWC", "48 89 5C 24 10 55 56 57 48 83 EC 70 65", [this](mem ptr)
	{
		g_get_screen_coords_from_world_coords = ptr.as<decltype(g_get_screen_coords_from_world_coords)>();
	});
	
	main_batch.add("GPBC", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2", [this](mem ptr)
	{
		g_get_ped_bone_coords = ptr.as<decltype(g_get_ped_bone_coords)>();
	});
	
	main_batch.add("GGCC", "8B 90 ? ? ? ? 89 13", [this](mem ptr)
	{
		g_get_gameplay_cam_coords = ptr.sub(0xE).as<decltype(g_get_gameplay_cam_coords)>();
	});
	
	main_batch.add("GFRCR", "48 89 5C 24 08 57 48 83 EC 30 8B DA 48 8B F9 E8 ? ? ? ? 48 8D 4C 24 20 44", [this](mem ptr)
	{
		g_get_final_rendered_cam_rot = ptr.as<decltype(g_get_final_rendered_cam_rot)>();
	});
	
	main_batch.add("GL", "48 8D 0D ? ? ? ? E8 ? ? ? ? 8B 0D ? ? ? ? 48 8B 5C 24 30", [this](mem ptr)
	{
		g_gxt_labels = ptr.mov().as<decltype(g_gxt_labels)>();
	});
	
	main_batch.add("GGLFT", "48 89 5C 24 08 57 48 83 EC 20 48 8B DA 48 8B F9 48 85 D2 75 44", [this](mem ptr)
	{
		g_get_gxt_label_from_table = ptr.as<decltype(g_get_gxt_label_from_table)>();
	});
	
	main_batch.add("GJGLFT", "48 83 EC 28 E8 ? ? ? ? 48 85 C0 75 34", [this](mem ptr)
	{
		g_get_joaated_gxt_label_from_table = ptr.as<decltype(g_get_joaated_gxt_label_from_table)>();
	});
	
	main_batch.add("RC", "E8 ? ? ? ? E9 B1 FD FF FF", [this](mem ptr)
	{
		g_request_control = ptr.call().as<decltype(g_request_control)>();
	});
	
	main_batch.add("GCP", "48 89 5C 24 08 48 89 74 24 18 89 54 24 10 57 48 83 EC 40 48", [this](mem ptr)
	{
		g_get_connection_peer = ptr.as<decltype(g_get_connection_peer)>();
	});
	
	main_batch.add("GPBSI", "76 E0 8B 4C 24 30 E8", [this](mem ptr)
	{
		g_get_peer_by_security_id = ptr.add(7).rip().as<decltype(g_get_peer_by_security_id)>();
	});
	
	main_batch.add("PEP", "4C 8B 35 ? ? ? ? B8", [this](mem ptr)
	{
		g_ped_pool = ptr.mov().as<decltype(g_ped_pool)>();
	});

	main_batch.add("VEP", "4C 8B 25 ? ? ? ? 8B 29", [this](mem ptr)
	{
		g_vehicle_pool = ptr.mov().as<decltype(g_vehicle_pool)>();
	});

	main_batch.add("PRP&PP", "48 8B 05 ? ? ? ? 0F B7 50 10 48 8B 05", [this](mem ptr)
	{
		g_prop_pool = ptr.mov().as<decltype(g_prop_pool)>();
		g_pickup_pool = ptr.add(14).rip().as<decltype(g_pickup_pool)>();
	});

	main_batch.add("VA", "48 8B 0D ? ? ? ? 45 33 C9 BA ? ? ? ? 41", [this](mem ptr)
	{
		g_vehicle_allocator = ptr.mov().as<decltype(g_vehicle_allocator)>();
	});

	main_batch.add("OIO", "FF 50 50 48 83 C7 08", [this](mem ptr)
	{
		g_object_ids_offset = ptr.add(0xF).as<decltype(g_object_ids_offset)>();
	});

	main_batch.add("WMSB", "48 85 C0 0F 84 F8 02 00 00 8B 48 50", [this](mem ptr)
	{
		g_world_model_spawn_bypass = ptr.as<decltype(g_world_model_spawn_bypass)>();
	});

	main_batch.add("CTSHP", "48 8B C8 FF 52 30 84 C0 74 05 48", [this](mem ptr)
	{
		g_model_spawn_bypass = ptr.add(8).as<decltype(g_model_spawn_bypass)>();
	});

	main_batch.add("CPTN", "E8 ? ? ? ? EB 28 48 8B 8F A0 10 00 00", [this](mem ptr)
	{
		g_clear_ped_task_network = ptr.call().as<decltype(g_clear_ped_task_network)>();
	});

	main_batch.add("MO", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8B F8 48", [this](mem ptr)
	{
		g_migrate_object = ptr.as<decltype(g_migrate_object)>();
	});

	main_batch.add("HSR", "E8 ? ? ? ? 48 8B 93 08 05 00 00 89 BB 04 05 00 00", [this](mem ptr)
	{
		g_http_start_request = ptr.call().as<decltype(g_http_start_request)>();
	});

	main_batch.add("WD", "48 8D 0D ? ? ? ? 48 98 4C 8B C6", [this](mem ptr)
	{
		g_waypoint_data = ptr.mov().as<decltype(g_waypoint_data)>();
	});

	main_batch.add("TS", "48 8D 0D ? ? ? ? E8 ? ? ? ? 8B 45 EC", [this](mem ptr)
	{
		g_texture_store = ptr.mov().as<decltype(g_texture_store)>();
	});

	main_batch.add("YFR", "48 89 5C 24 08 48 89 6C 24 18 48 89 7C 24 20 41 54", [this](mem ptr)
	{
		g_ytd_file_register = ptr.as<decltype(g_ytd_file_register)>();
	});

	main_batch.add("NPMI", "41 56 48 83 EC ? 48 8B F1 B9 ? ? ? ? 49 8B F9 41 8B E8 4C 8B F2 E8", [this](mem ptr)
	{
		g_network_player_mgr_init = ptr.sub(0x13).as<decltype(g_network_player_mgr_init)>();
	});

	main_batch.add("NPMS", "48 8D 9F ? ? ? ? EB ? 48 8B 13 48 85 D2 74 ? 48 8B CB E8 ? ? ? ? 48 83 7B ? ? 75 ? 48 8D 9F", [this](mem ptr)
	{
		g_network_player_mgr_shutdown = ptr.sub(0x1A).as<decltype(g_network_player_mgr_shutdown)>();
	});

	main_batch.add("FR", "3B 0D ? ? ? ? 73 17", [this](mem ptr)
	{
		g_friend_registry = ptr.lea().as<decltype(g_friend_registry)>();
	});

	main_batch.add("SI", "48 8B D3 48 8D 4C 24 ? 48 69 D2", [this](mem ptr)
	{
		g_sc_info = ptr.sub(4).rip().as<decltype(g_sc_info)>();
	});

	main_batch.add("NPM", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07", [this](mem ptr)
	{
		g_network_player_mgr = ptr.mov().as<decltype(g_network_player_mgr)>();
	});

	main_batch.add("NOM", "48 8B 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 33 FF 4C 8B F0", [this](mem ptr)
	{
		g_network_object_mgr = ptr.mov().as<decltype(g_network_object_mgr)>();
	});

	main_batch.add("N", "48 8B 0D ? ? ? ? 45 33 C9 48 8B D7", [this](mem ptr)
	{
		g_network = ptr.mov().as<decltype(g_network)>();
	});

	main_batch.add("PF", "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52", [this](mem ptr)
	{
		g_ped_factory = ptr.mov().as<decltype(g_ped_factory)>();
	});

	main_batch.add("SPT", "48 8B 1D ? ? ? ? 41 83 F8 FF", [this](mem ptr)
	{
		g_scr_program_table = ptr.mov().as<decltype(g_scr_program_table)>();
	});

	main_batch.add("NH", "48 8D 0D ? ? ? ? 48 8B 14 FA", [this](mem ptr)
	{
		g_init_native_table = ptr.sub(37).as<decltype(g_init_native_table)>();
		g_native_registration_table = ptr.mov().as<decltype(g_native_registration_table)>();
		g_get_native_handler = ptr.add(12).rip().as<decltype(g_get_native_handler)>();
	});

	main_batch.add("SC", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01", [this](mem ptr)
	{
		g_swap_chain = ptr.mov().as<decltype(g_swap_chain)>();
	});

	main_batch.add("HT", "4C 03 05 ? ? ? ? EB 03", [this](mem ptr)
	{
		g_hash_table = ptr.mov().as<decltype(g_hash_table)>();
	});

	main_batch.add("G", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF", [this](mem ptr)
	{
		g_globals = ptr.mov().as<decltype(g_globals)>();
	});

	main_batch.add("TI", "8B 15 ? ? ? ? 48 8B 05 ? ? ? ? FF C2 89 15 ? ? ? ? 48 8B 0C F8", [this](mem ptr)
	{
		g_thread_id = ptr.lea().as<decltype(g_thread_id)>();
	});

	main_batch.add("TC", "FF 0D ? ? ? ? 48 8B D9", [this](mem ptr)
	{
		g_thread_count = ptr.lea().as<decltype(g_thread_count)>();
	});

	main_batch.add("GV&OV", "8B C3 33 D2 C6 44 24 20", [this](mem ptr)
	{
		g_game_version = ptr.add(0x24).rip().as<decltype(g_game_version)>();
		g_online_version = ptr.add(0x24).rip().add(0x20).as<decltype(g_online_version)>();
	});

	main_batch.add("GSU", "48 8D 0D ? ? ? ? BA 01 00 00 00 74 05", [this](mem ptr)
	{
		g_game_skeleton = ptr.mov().as<decltype(g_game_skeleton)>();
	});

	main_batch.add("MA", "48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 89 50 ? 55 41 54 41 55 41 56 41 57 48 8D A8", [this](mem ptr)
	{
		g_matchmaking_advertise = ptr.as<decltype(g_matchmaking_advertise)>();
	});

	main_batch.add("MUA", "EB 21 B9 01 00 00 00 87 4B 28", [this](mem ptr)
	{
		g_matchmaking_unadvertise = ptr.sub(4).rip().as<decltype(g_matchmaking_unadvertise)>();
	});

	main_batch.add("SDR", "E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 4D 28", [this](mem ptr)
	{
		g_session_detail_response = ptr.call().as<decltype(g_session_detail_response)>();
	});

	main_batch.add("RC", "48 83 EC 28 83 3D ? ? ? ? ? 75 10", [this](mem ptr)
	{
		g_region_code = ptr.add(16).rip().as<decltype(g_region_code)>();
	});

	main_batch.add("TCKF", "48 83 EC 18 48 8B 0D", [this](mem ptr)
	{
		g_timecycle_keyframe_override = ptr.as<decltype(g_timecycle_keyframe_override)>();
	});

	main_batch.add("IDC", "40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 D0", [this](mem ptr)
	{
		g_invalid_decal_crash = ptr.as<decltype(g_invalid_decal_crash)>();
	});

	main_batch.add("IPCD", "85 D2 0F 84 ? ? ? ? 48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 48 89 78 ? 41 56 48 83 EC ? 8D 42", [this](mem ptr)
	{
		g_invalid_ped_component_draw = ptr.as<decltype(g_invalid_ped_component_draw)>();
	});

	main_batch.add("SLC", "0F 29 70 E8 0F 29 78 D8 48 8B F9 48 8B CA", [this](mem ptr)
	{
		g_searchlight_crash = ptr.sub(0x1E).as<decltype(g_searchlight_crash)>();
		g_get_searchlight = ptr.add(0x28).rip().as<decltype(g_get_searchlight)>();
	});

	main_batch.add("TPO", "0F 88 ? ? ? ? 75 34", [this](mem ptr)
	{
		g_task_parachute_object = ptr.sub(6).as<decltype(g_task_parachute_object)>();
	});

	main_batch.add("TAC", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? ? 41 8B D8 8B F2", [this](mem ptr)
	{
		g_task_ambient_clips = ptr.as<decltype(g_task_ambient_clips)>();
	});

	main_batch.add("SCM", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 81 EC 80 00 00 00 48 8B E9 48 8B CA", [this](mem ptr)
	{
		g_send_chat_message = ptr.as<decltype(g_send_chat_message)>();
	});

	main_batch.add("SPDM", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 20 BF 01 00 00 00", [this](mem ptr)
	{
		g_serialize_player_data_msg = ptr.as<decltype(g_serialize_player_data_msg)>();
	});

	main_batch.add("SJRM", "E8 ? ? ? ? 84 C0 0F 84 9B 00 00 00 49 8D 8F 50 11 00 00", [this](mem ptr)
	{
		g_serialize_join_request_message = ptr.as<decltype(g_serialize_join_request_message)>();
	});

	main_batch.add("STOPVT", "40 55 53 57 41 56 48 8B EC 48 83 EC 68", [this](mem ptr)
	{
		g_serialize_take_off_ped_variation_task = ptr.as<decltype(g_serialize_take_off_ped_variation_task)>();
	});

	main_batch.add("SPT", "40 55 53 56 57 41 54 48 8B", [this](mem ptr)
	{
		g_serialize_parachute_task = ptr.as<decltype(g_serialize_parachute_task)>();
	});

	main_batch.add("FPC2", "48 8B C4 55 53 57 48 8D A8 28 FF FF FF 48 81 EC C0 01 00 00 0F 28", [this](mem ptr)
	{
		g_fragment_physics_crash_2 = ptr.as<decltype(g_fragment_physics_crash_2)>();
	});

	main_batch.add("DHM", "48 89 05 ? ? ? ? EB 07 48 89 1D ? ? ? ? 48 8B CB", [this](mem ptr)
	{
		g_draw_handler_mgr = ptr.mov().as<decltype(g_draw_handler_mgr)>();
	});

	main_batch.add("RP", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC 80 00 00 00 48 8B FA", [this](mem ptr)
	{
		g_render_ped = ptr.as<PVOID*>();
	});

	main_batch.add("RE", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 41 54 41 55 41 56 41 57 48 83 EC 70 0F BA", [this](mem ptr)
	{
		g_render_entity = ptr.as<PVOID*>();
	});

	main_batch.add("RBP", "48 89 5C 24 08 4C 89 44 24 18 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48", [this](mem ptr)
	{
		g_render_big_ped = ptr.as<PVOID*>();
	});

	main_batch.add("RR", "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? ? 48 8B FA 48 8B D9 74 13", [this](mem ptr)
	{
		g_remove_reference = ptr.as<decltype(g_remove_reference)>();
	});

	main_batch.add("SGSBGH", "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 05 ? ? ? ? 48 8D 4C 24", [this](mem ptr)
	{
		g_start_get_session_by_gamer_handle = ptr.call().as<decltype(g_start_get_session_by_gamer_handle)>();
	});

	main_batch.add("SPBGH", "40 53 48 83 EC 20 8B 15 ? ? ? ? 48 8B D9 85 D2 75 0F 8B CA E8 ? ? ? ? 8B 15 ? ? ? ? EB 02 32 C0 84 C0 74 14", [this](mem ptr)
	{
		g_show_profile_by_gamer_handle = ptr.as<decltype(g_show_profile_by_gamer_handle)>();
	});

	main_batch.add("SFR", "48 89 5C 24 08 57 48 83 EC 20 48 8B F9 B1 01 48 8B DA E8 ? ? ? ? 84 C0 74 18", [this](mem ptr)
	{
		g_send_friend_request = ptr.as<decltype(g_send_friend_request)>();
	});

	main_batch.add("IPBGH", "E8 ? ? ? ? 4C 8D 05 ? ? ? ? 48 8D 15 ? ? ? ? E9", [this](mem ptr)
	{
		g_invite_player_by_gamer_handle = ptr.call().as<decltype(g_invite_player_by_gamer_handle)>();
	});

	main_batch.add("JSBI", "89 6C 24 28 4C 89 74 24 20 E8 ? ? ? ? EB 02 32 C0 48 8B 5C 24 40", [this](mem ptr)
	{
		g_join_session_by_info = ptr.add(10).rip().as<decltype(g_join_session_by_info)>();
	});

	main_batch.add("NC", "48 8B 0D ? ? ? ? 45 33 C9 48 8B D7", [this](mem ptr)
	{
		g_network_config = ptr.mov().as<decltype(g_network_config)>();
	});

	main_batch.add("3DP", "4C 8B 15 ? ? ? ? F3 0F 11 45", [this](mem ptr)
	{
		m_ui_3d_draw_manager = ptr.read_instruction(ptr.as<uint64_t>(), 3, 7);
		m_push_scene_present_to_manager = ptr.read_instruction(ptr.as<uint64_t>() + 0x5D, 1, 5);
		m_add_element_to_scene = ptr.read_instruction(ptr.as<uint64_t>() + 0x9F, 1, 5);
		m_set_scene_element_lightning = ptr.read_instruction(ptr.as<uint64_t>() + 0xE4, 1, 5);
	});

	main_batch.add("NGP", "48 83 EC 28 33 C0 38 05 ? ? ? ? 74 0A", [this](mem ptr)
	{
		g_net_game_player = ptr.as<decltype(g_net_game_player)>();
	});

	main_batch.add("WPCDN", "40 53 48 83 EC 20 84 C9 75", [this](mem ptr)
	{
		g_set_in_spectator_mode = ptr.as<decltype(g_set_in_spectator_mode)>();
	});

	main_batch.add("WPCDN", "48 8B C4 48 89 58 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 B0 48 81 EC 50 01 00 00 4C", [this](mem ptr)
	{
		g_write_player_camera_data_node = ptr.as<decltype(g_write_player_camera_data_node)>();
	});

	main_batch.add("WPGSDN", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 30 0F B7 81", [this](mem ptr)
	{
		g_write_player_game_state_data_node = ptr.as<decltype(g_write_player_game_state_data_node)>();
	});

	main_batch.add("WVPMDN", "48 89 4C 24 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 68 4C 8B A9", [this](mem ptr)
	{
		g_write_vehicle_proximity_migration_data_node = ptr.as<decltype(g_write_vehicle_proximity_migration_data_node)>();
	});

	main_batch.add("HRGC", "74 74 33 FF 45 33 F6", [this](mem ptr)
	{
		g_handle_remove_gamer_cmd = ptr.sub(0x3B).as<decltype(g_handle_remove_gamer_cmd)>();
	});

	main_batch.add("TSE", "48 8B C4 48 89 58 08 44 89 48 20 55 56 57 48 83 EC 30", [this](mem ptr)
	{
		g_trigger_script_event = ptr.as<decltype(g_trigger_script_event)>();
	});

	main_batch.add("TCA", "48 8D 0D ? ? ? ? E8 ? ? ? ? 44 88 64 24 30 4C 8D 8C 24 60 02 00 00", [this](mem ptr)
	{
		g_train_config_array = ptr.mov().as<decltype(g_train_config_array)>();
	});

	main_batch.add("IPP", "4C 8B 05 ? ? ? ? 4C 0F BF 0B", [this](mem ptr)
	{
		g_interior_proxy_pool = ptr.mov().as<decltype(g_interior_proxy_pool)>();
	});

	main_batch.add("RAU", "48 89 5C 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 30 48 8B 05", [this](mem ptr)
	{
		g_received_array_update = ptr.as<decltype(g_received_array_update)>();
	});

	main_batch.add("RPI", "49 8B 80 A8 10 00 00 48 85 C0 74 0C", [this](mem ptr)
	{
		g_receive_pickup = ptr.as<decltype(g_receive_pickup)>();
	});

	main_batch.add("RNM", "48 83 EC 20 4C 8B 71 50 33 ED", [this](mem ptr)
	{
		g_receive_net_message = ptr.sub(0x19).as<decltype(g_receive_net_message)>();
	});

	main_batch.add("FI64TC", "48 83 EC 38 44 88", [this](mem ptr)
	{
		g_format_i64_to_cash = ptr.as<decltype(g_format_i64_to_cash)>();
	});

	main_batch.add("QP", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 44 89 48 20 57 41 54 41 55 41 56 41 57 48 83 EC 30 4C", [this](mem ptr)
	{
		g_queue_packet = ptr.as<decltype(g_queue_packet)>();
	});

	main_batch.add("SP", "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 48 20 55 41 54 41 55 41 56 41 57 48 8D A8 98", [this](mem ptr)
	{
		g_send_packet = ptr.as<decltype(g_send_packet)>();
	});

	main_batch.add("GPR", "48 8B C8 33 C0 48 85 C9 74 0A 44 8B C3 8B D7 E8", [this](mem ptr)
	{
		g_give_pickup_rewards = ptr.sub(0x28).as<decltype(g_give_pickup_rewards)>();
	});

	main_batch.add("GBNV", "48 83 EC 28 66 83 79 30 00", [this](mem ptr)
	{
		g_get_blip_name_value = ptr.as<decltype(g_get_blip_name_value)>();
	});

	main_batch.add("GMI", "41 3B 0A 74 54", [this](mem ptr)
	{
		g_get_model_info = ptr.sub(0x2E).as<decltype(g_get_model_info)>();
	});

	main_batch.add("BNA", "48 89 5C 24 18 48 89 54 24 10 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 40", [this](mem ptr)
	{
		g_broadcast_net_array = ptr.as<decltype(g_broadcast_net_array)>();
	});

	main_batch.add("RNC", "E8 ? ? ? ? 83 BB 70 10 00 00 00", [this](mem ptr)
	{
		g_reset_network_complaints = ptr.call().as<decltype(g_reset_network_complaints)>();
	});

	main_batch.add("CANF", "E9 36 01 00 00 33 D2 8B CB", [this](mem ptr)
	{
		g_can_access_network_features = ptr.add(10).rip().as<decltype(g_can_access_network_features)>();
	});

	main_batch.add("WF", "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55 41 56 41 57 48 8D 68 A1 48 81 EC D0 00 00 00 0F 29 70 D8 45", [this](mem ptr)
	{
		g_cweapon_fire = ptr.as<cweapon_fire>();
	});

	main_batch.add("ECM", "48 8B 0D ? ? ? ? 33 D2 E8 ? ? ? ? 45 33 C0 48 8D 4C 24 30 41 8D 70 04 89 44 24 30 8B D6 E8 ? ? ? ? 8B D8 E8", [this] (mem ptr)
	{
		g_extra_content_manager = ptr.mov().as<decltype(g_extra_content_manager)>();
	});

	main_batch.add("ECMACF", "4C 8B C3 E8 ? ? ? ? 48 8D 1D ? ? ? ? 48 8D", [this] (mem ptr)
	{
		g_CExtraContentManager_AddContentFolder = ptr.sub(70).as<decltype(g_CExtraContentManager_AddContentFolder)>();
	});

	main_batch.add("ECMLC", "48 8B CE C6 80 F0 00 00 00 01 E8", [this] (mem ptr)
	{
		g_CExtraContentManager_LoadContent = ptr.sub(13).as<decltype(g_CExtraContentManager_LoadContent)>();
	});
	main_batch.add("IMSV", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 45 0F", [this](mem ptr)
	{
		g_is_matchmaking_session_valid = ptr;
	});

	main_batch.add("FEE", "48 8B 5C 24 40 48 8B 6C 24 48 48 8B 74 24 50 48 8B 7C 24 58 48 83 C4 30 41 5E C3 48 8B 0D", [this](mem ptr)
	{
		g_free_event_error = ptr.add(0x31).as<decltype(g_free_event_error)>();
	});

	main_batch.add("BP", "74 73 FF 90 ? ? ? ? 8B D5 4C 8B 00 48 8B C8 41 FF 50 30", [this](mem ptr)
	{
		g_broadcast_patch = ptr;
	});

	main_batch.add("CW", "74 44 E8 ? ? ? ? 80 65 2B F8 48 8D 0D ? ? ? ? 48 89 4D 17 48 89 7D 1F 89 7D 27 C7 45", [this](mem ptr)
	{
		g_creator_warp_cheat_triggered_patch = ptr;
	});

	main_batch.add("EP", "E8 ? ? ? ? 48 8D 4C 24 20 E8 ? ? ? ? 4C 8D 9C 24 80 01 00 00", [this](mem ptr)
	{
		g_explosion_patch = ptr;
	});

	main_batch.add("BE", "0F 85 EE 00 00 00 84 C0", [this](mem ptr)
	{
		g_blame_explode = ptr;
	});

	main_batch.add("RMS", "E8 ? ? ? ? 33 C0 87 83 90 02 00 00", [this](mem ptr)
	{
		g_report_myself_sender = ptr.call().as<decltype(g_report_myself_sender)>();
	});

	main_batch.add("BENBP", "83 25 ? ? ? ? 00 89 0D ? ? ? ? 85 C9", [this](mem ptr)
	{
		g_be_network_bail_patch = ptr.add(17).rip().call().as<decltype(g_be_network_bail_patch)>();
	});

	main_batch.add("ASAP", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B7 49 30 48 8B F2 E8 ? ? ? ? 33 DB 48 8B F8 48 85 C0 74 35", [this](mem ptr)
	{
		g_activate_special_ability_patch = ptr.as<decltype(g_activate_special_ability_patch)>();
	});

	main_batch.add("CT", "48 3B F8 74 ? 8B 1D", [this](mem ptr)
	{
		g_crash_trigger = ptr;
	});

	main_batch.add("MWL", "8B 43 6C 89 05", [this](mem ptr)
	{
		g_max_wanted_level = ptr;
	});

	main_batch.add("SVM1", "3b 0a 0f 83 ? ? ? ? 48 ff c7", [this](mem ptr)
	{
		g_script_vm_patch_1 = ptr;
	});

	main_batch.add("SVM2", "3b 0a 0f 83 ? ? ? ? 49 03 fa", [this](mem ptr)
	{
		g_script_vm_patch_2 = ptr;
	});

	main_batch.add("SVM3&4", "3b 11 0f 83 ? ? ? ? 48 ff c7", [this](mem ptr)
	{
		g_script_vm_patch_3 = ptr;
		g_script_vm_patch_4 = ptr.add(0x1C);
	});

	main_batch.add("SVM5&6", "3b 11 0f 83 ? ? ? ? 49 03 fa", [this](mem ptr)
	{
		g_script_vm_patch_5 = ptr;
		g_script_vm_patch_6 = ptr.add(0x26);
	});

	main_batch.run("GTA5.exe");
	const auto end = steady_clock::now();
	const duration<double> diff = end - start;
	LOG(info, "Found: {} out of {} signatures. Scan time took {:2f}s with a per sig avg. of {:2f}s", main_batch.m_found_sig_count, main_batch.m_total_sig_count, diff.count(), diff.count() / main_batch.m_total_sig_count);
}

void pointers::ScanScriptHookPointer()
{
	if (GetModuleHandleA("ScriptHookV.dll") != nullptr)
	{
		LOG(info, "ScriptHookV.dll is loaded. Scanning for pointers...");

		static pattern_batch scripthook_batch{};

		/*scripthook_batch.add("OC", "EB 3A 48 8D 0D", [this](mem ptr)
		{
			g_onlinecheck = ptr;
		});*/

		scripthook_batch.add("PS", "41 81 FA 2C 23 82 11", [this](mem ptr)
		{
			g_poolstuff = ptr;
		});

		scripthook_batch.add("MPS", "B8 00 01 00 00 3B C3", [this](mem ptr)
		{
			g_morepoolstuff = ptr;
		});

		scripthook_batch.run("ScriptHookV.dll");
	}
}