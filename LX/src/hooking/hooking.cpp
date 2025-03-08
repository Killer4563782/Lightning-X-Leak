#include "hooking/hooking.hpp"

#include "memory/pointers.h"

hooking::hooking() 
{
	m_swapchain_hook = new vmt_hook(*g_pointers->g_swap_chain, hooks::swapchain_num_funcs); 
	m_swapchain_hook->hook(hooks::swapchain_present_index, &hooks::present);
	m_swapchain_hook->hook(hooks::swapchain_resizebuffers_index, &hooks::resizebuffers);

	for (auto& detour_hook_helper : m_detour_hook_helpers)
	{
		detour_hook_helper.m_detour_hook->set_target_and_create_hook(detour_hook_helper.m_on_hooking_available());
	}

	detour_hook_helper::add<hooks::run_script_threads>("SH", g_pointers->g_run_script_threads);
	detour_hook_helper::add<hooks::get_label_text>("GLT", g_pointers->g_get_label_text);

	detour_hook_helper::add<hooks::gta_thread_start>("GTS", g_pointers->g_gta_thread_start);
	detour_hook_helper::add<hooks::gta_thread_kill>("GTK", g_pointers->g_gta_thread_kill);
	detour_hook_helper::add<hooks::init_native_tables>("INT", g_pointers->g_init_native_table);
	detour_hook_helper::add<hooks::script_vm>("SVM", g_pointers->g_script_vm);
	detour_hook_helper::add<hooks::can_access_network_features>("CANF", g_pointers->g_can_access_network_features);
	
	detour_hook_helper::add<hooks::assign_physical_index>("API", g_pointers->g_assign_physical_index);
	detour_hook_helper::add<hooks::task_jump_constructor>("TJC", g_pointers->g_c_task_jump_constructor);
	detour_hook_helper::add<hooks::task_fall_constructor>("TFC", g_pointers->g_c_task_fall_constructor);

	detour_hook_helper::add<hooks::network_player_mgr_init>("NPMI", g_pointers->g_network_player_mgr_init);
	detour_hook_helper::add<hooks::network_player_mgr_shutdown>("NPMS", g_pointers->g_network_player_mgr_shutdown);

	detour_hook_helper::add<hooks::received_event>("RE", g_pointers->g_received_event);
	detour_hook_helper::add<hooks::receive_net_message>("RNM", g_pointers->g_receive_net_message);
	detour_hook_helper::add<hooks::get_network_event_data>("GNED", g_pointers->g_get_network_event_data);

	detour_hook_helper::add<hooks::write_player_game_state_data_node>("WPGSDN", g_pointers->g_write_player_game_state_data_node);

	detour_hook_helper::add<hooks::invalid_decal>("IDC", g_pointers->g_invalid_decal_crash);
	detour_hook_helper::add<hooks::invalid_ped_component_draw>("IPCD", g_pointers->g_invalid_ped_component_draw);
	detour_hook_helper::add<hooks::task_parachute_object>("TPO", g_pointers->g_task_parachute_object);
	detour_hook_helper::add<hooks::task_ambient_clips>("TAC", g_pointers->g_task_ambient_clips);
	detour_hook_helper::add<hooks::can_apply_data>("CAD", g_pointers->g_can_apply_data);
	detour_hook_helper::add<hooks::searchlight_crash>("SLC", g_pointers->g_searchlight_crash);

	detour_hook_helper::add<hooks::send_chat_message>("SCM", g_pointers->g_send_chat_message);
	detour_hook_helper::add<hooks::serialize_player_data_msg>("SJPD", g_pointers->g_serialize_player_data_msg);
	detour_hook_helper::add<hooks::serialize_join_request_message>("SJRM", g_pointers->g_serialize_join_request_message);

	detour_hook_helper::add<hooks::broadcast_net_array>("BNA", g_pointers->g_broadcast_net_array);

	detour_hook_helper::add<hooks::serialize_take_off_ped_variation_task>("STOPVT", g_pointers->g_serialize_take_off_ped_variation_task);
	detour_hook_helper::add<hooks::get_dlc_hash>("GDH", g_pointers->g_get_dlc_hash);
	detour_hook_helper::add<hooks::serialize_parachute_task>("SPT", g_pointers->g_serialize_parachute_task);
	detour_hook_helper::add<hooks::SMPACreateStub>("SCS", g_pointers->g_smpa_create_stub);
	detour_hook_helper::add<hooks::ReadGameConfig>("RGC", g_pointers->g_read_game_config);
	detour_hook_helper::add<hooks::CreatePool>("CP", g_pointers->g_create_pool);
	detour_hook_helper::add<hooks::GetPoolItem>("GPI", g_pointers->g_get_pool_item);
	detour_hook_helper::add<hooks::http_start_request>("HSR", g_pointers->g_http_start_request);
	detour_hook_helper::add<hooks::fragment_physics_crash_2>("FPC2", g_pointers->g_fragment_physics_crash_2);

	detour_hook_helper::add<hooks::received_array_update>("RAU", g_pointers->g_received_array_update);
	detour_hook_helper::add<hooks::receive_pickup>("RPI", g_pointers->g_receive_pickup);

	detour_hook_helper::add<hooks::write_player_camera_data_node>("WPCDN", g_pointers->g_write_player_camera_data_node);

	detour_hook_helper::add<hooks::write_vehicle_proximity_migration_data_node>("WVPMDN", g_pointers->g_write_vehicle_proximity_migration_data_node);

	detour_hook_helper::add<hooks::log_error_message_box>("E0MBH", g_pointers->g_error_message_box);

	detour_hook_helper::add<hooks::allocate_memory_reliable>("AMR", g_pointers->g_allocate_memory_reliable);

	detour_hook_helper::add<hooks::render_ped>("RP", g_pointers->g_render_ped);
	detour_hook_helper::add<hooks::render_entity>("RE", g_pointers->g_render_entity);
	detour_hook_helper::add<hooks::render_big_ped>("RBP", g_pointers->g_render_big_ped);

	detour_hook_helper::add<hooks::format_int64_to_cash>("FI64TC", g_pointers->g_format_i64_to_cash);
	detour_hook_helper::add<hooks::player_tags_on_minimap>("PTOM", g_pointers->g_get_blip_name_value);
	detour_hook_helper::add<hooks::c_weapon_fire>("WF", g_pointers->g_cweapon_fire);
	detour_hook_helper::add<hooks::rage_rlScMatchmaking_Advertise>("MA", g_pointers->g_matchmaking_advertise);
	detour_hook_helper::add<hooks::rage_rlScMatchmaking_Unadvertise>("MUA", g_pointers->g_matchmaking_unadvertise);
	detour_hook_helper::add<hooks::rage_netTransactor_SendResponse_rage_rlSessionDetailResponse>("SRRRSDR", g_pointers->g_session_detail_response);

	detour_hook_helper::add<hooks::update_timecycle_keyframe_data>("UTCKD", g_pointers->g_timecycle_keyframe_override);
}

hooking::~hooking()
{
	if (m_enabled)
	{
		disable();
	}
}

void hooking::enable()
{
	m_swapchain_hook->enable();
	m_og_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_pointers->g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&hooks::wndproc)));

	for (const auto& detour_hook_helper : m_detour_hook_helpers)
	{
		detour_hook_helper.m_detour_hook->enable();
	}

	MH_ApplyQueued();

	m_enabled = true;
}

void hooking::disable()
{
	m_enabled = false;

	for (const auto& detour_hook_helper : m_detour_hook_helpers)
	{
		detour_hook_helper.m_detour_hook->disable();
	}

	SetWindowLongPtrW(g_pointers->g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_og_wndproc));
	m_swapchain_hook->disable();
	delete m_swapchain_hook; 

	MH_ApplyQueued();

	m_detour_hook_helpers.clear();
}

hooking::detour_hook_helper::~detour_hook_helper() = default;

void hooking::detour_hook_helper::enable_hook_if_hooking_is_already_running() const
{
	if (g_hooking && g_hooking->m_enabled)
	{
		if (m_on_hooking_available)
		{
			m_detour_hook->set_target_and_create_hook(m_on_hooking_available());
		}

		m_detour_hook->enable();
		MH_ApplyQueued();
	}
}
