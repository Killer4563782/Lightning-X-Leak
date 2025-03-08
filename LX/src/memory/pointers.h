#pragma once
#include "mem.h"
#include "rage/gta/net_array.hpp"
#include <rage/gta/script_handler.hpp>
#include <network/grmShaderGroup.hpp>
#include <network/rl_matching.h>
#include <network/netstatus.h>
#include <rlSessionDetail.hpp>

class ScGameInfo;
class CNetworkObjectMgr;
class CExtraContentManager;

namespace rage
{
	class rlPresenceEventInviteAccepted;
	class netEventMgr;
	class gameSkeleton;
	class CExtraContentManager;

	struct waypoint_data
	{
		vector4<float>* m_points;
		char _0x0008[0x3C];
		uint32_t m_count;
	};
}

class generic_pool;
class vehicle_pool;
class CWeaponInfoManager
{
public:
	char field_0x0[16][4];
	rage::atArray<CWeaponInfo*> m_item_infos;
};

inline bool g_was_arx_guardit_present{};

class pointers
{
public:
	pointers();
	~pointers() = default;
	void ScanScriptHookPointer(); 
public: 
	using run_script_threads = bool (*)(uint32_t ops_to_execute);
	using pointer_to_handle = Entity(*)(rage::CEntity* ptr);
	using handle_to_pointer = rage::CDynamicEntity* (*)(Entity);
	// Bitbuffer read/write START
	using read_bitbuf_dword = bool (*)(rage::datBitBuffer* buffer, PVOID read, int bits);
	using read_bitbuf_string = bool (*)(rage::datBitBuffer* buffer, char* read, int bits);
	using read_bitbuf_bool = bool (*)(rage::datBitBuffer* buffer, bool* read, int bits);
	using read_bitbuf_array = bool (*)(rage::datBitBuffer* buffer, PVOID read, int bits, int unk);
	using write_bitbuf_qword = bool (*)(rage::datBitBuffer* buffer, uint64_t val, int bits);
	using write_bitbuf_dword = bool (*)(rage::datBitBuffer* buffer, uint32_t val, int bits);
	using write_bitbuf_int64 = bool (*)(rage::datBitBuffer* buffer, int64_t val, int bits);
	using write_bitbuf_int32 = bool (*)(rage::datBitBuffer* buffer, int32_t val, int bits);
	using write_bitbuf_bool = bool (*)(rage::datBitBuffer* buffer, bool val, int bits);
	using write_bitbuf_array = bool (*)(rage::datBitBuffer* buffer, void* val, int bits, int unk);

	// Bitbuffer read/write END

	using c_task_jump_constructor = int64_t(*)(uint64_t This, int flags);
	using c_task_fall_constructor = void* (*)(uint64_t This, uint32_t flags);
	using run_async_module_request = void(*)(uint64_t* module);
	using get_connection_peer = rage::netConnectionPeer* (*)(rage::netConnectionManager* manager, uint32_t peer_id);
	using has_interval_elapsed = bool(*)(uint32_t timestamp, uint32_t interval);
	using dispatch_event = bool(*)(uint64_t This, rage::netConnectionManager* p_con_mgr, rage::netConnection::InFrame* p_event);
	using script_vm = rage::eThreadState(*)(uint64_t* stack, int64_t** scr_globals, rage::scrProgram* program, rage::scrThreadContext* ctx);
	using sc_get_game_info_index = int(*)(const char* string_id, uint64_t unk, uint32_t game_id);
	using join_session_by_info = bool (*)(Network* network, rage::rlSessionInfo* info, int32_t k_slot_type, uint32_t join_flags, rage::rlGamerHandle* reserved_gamers, uint32_t num_reserved_gamers);
	using received_event = void(*)(rage::netEventMgr* p_event_mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, uint16_t id, int32_t index, int32_t handled_bitset, int32_t buffer_size, rage::datBitBuffer* buffer);
	using send_event_ack = void(*)(rage::netEventMgr* p_event_mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, int32_t index, int32_t handled_bitset);
	using get_net_object = rage::netObject* (*)(CNetworkObjectMgr* mgr, int16_t id, bool can_delete_be_pending);
	using received_clone_remove = void (*)(CNetworkObjectMgr*, CNetGamePlayer*, CNetGamePlayer*, int16_t, uint32_t);
	using received_clone_sync = NetObjectAckCode(*)(CNetworkObjectMgr* p_obj_mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, eNetObjectType object_type, uint16_t object_id, rage::datBitBuffer* buffer, uint16_t unknown, uint32_t timestamp);
	using receive_clone_create = bool(*)(CNetworkObjectMgr* p_obj_mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, eNetObjectType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp);
	using can_apply_data = bool(*)(rage::netSyncTree* p_sync_tree, rage::netObject* p_object);
	using get_sync_tree_for_type = rage::netSyncTree* (*)(CNetworkObjectMgr* p_obj_mgr, uint16_t sync_type);
	using get_entity_attached_to = rage::CDynamicEntity* (*)(rage::CDynamicEntity* entity);
	using get_host_array_handler_by_index = rage::netArrayHandlerBase* (*)(CGameScriptHandlerNetComponent* component, int index);
	using get_title_caption_error_message_box = const wchar_t* (*)(rage::joaat_t joaated_error_code);
	using connection_manager_try_free_memory = void(*)(rage::netConnectionManager* mgr);
	using remove_message_from_queue = void(*)(rage::netMessageQueue* queue, rage::netQueuedMessage* message);
	using remove_message_from_unacked_reliables = void(*)(void* list, uint16_t* unk);
	using get_scr_command_t = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable* This, uint64_t translated_hash);
	using get_screen_coords_from_world_coords = bool(*)(rage::vector3<float>* vec_world_pos, float* normalised_x, float* normalised_y);
	using get_ped_bone_coords = bool(*)(CPed* c_ped, rage::fvector3* out_buffer, uint32_t bone_mask);
	using get_gxt_label_from_table_t = const char* (*)(void* This, const char* label);
	using get_joaated_gxt_label_from_table_t = const char* (*)(void* This, rage::joaat_t hash);
	using request_control = void(*)(rage::netObject* net_object);
	using clear_ped_task_network = void (*)(CPed* ped, bool immediately);
	using migrate_object = void (*)(CNetGamePlayer* player, rage::netObject* object, int type);
	using ytd_file_register = uint32_t(*)(int* result, const char* file, bool quitOnBadVersion, const char* relativePath, bool quitIfMissing, bool overlayIfExists);
	using get_native_handler = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable* registration_table, rage::scrNativeHash hash);
	using send_chat_message = bool (*)(int64_t* send_chat_ptr, rage::rlGamerInfo* gamer_info, char* message, bool is_team);
	using remove_reference = void (*)(rage::fwRefAwareBase* object, void* reference);
	using start_get_session_by_gamer_handle = bool (*)(int profile_index, rage::rlGamerHandle* handles, int count, rage::rlSessionByGamerTaskResult* result, int unk, bool* success, rage::rlTaskStatus* state);
	using show_profile_by_gamer_handle = void (*)(rage::rlGamerHandle* handle);
	using send_friend_request = bool (*)(rage::rlGamerHandle* gamer_handle, const char* sz_message);
	using invite_player_by_gamer_handle = bool (*)(Network* network, rage::rlGamerHandle* handles, int handle_count, const char* subject, const char* msg, rage::rlTaskStatus* state);
	using get_net_game_player = CNetGamePlayer* (*) (uint32_t player);
	using write_player_game_state_data_node = bool (*)(rage::netObject* plr, CPlayerGameStateDataNode* node);
	using handle_remove_gamer_cmd = void* (*)(rage::snSession* session, rage::snPlayer* origin, rage::snMsgRemoveGamersFromSessionCmd* cmd);
	using trigger_script_event = void (*)(int event_group, int64_t* args, int arg_count, int player_bits, int event_id);
	using format_i64_to_cash = void (*)(int64_t cash, char* cash_string, int cash_string_size, bool use_commas);
	using get_gameplay_cam_coords = Vector3(*)();
	using get_final_rendered_cam_rot = Vector3(*)(int rotation_order);
	using send_invite_accepted_presence_event = void(*)(void* presence_struct, rage::rlPresenceEventInviteAccepted* invite, int flags);
	using queue_packet = bool (*)(rage::netConnectionManager* mgr, int msg_id, void* data, int size, int flags, void* unk);
	using send_packet = bool (*)(rage::netConnectionManager* mgr, rage::netPeerAddress* adde, int connection_id, void* data, int size, int flags);
	using give_pickup_rewards = void (*)(uint32_t receiving_players, uint32_t reward_hash);
	using get_blip_name_value = char*(*)(__int64 blip);
	using cweapon_fire = bool(*)(__int64 _this, __int64 params);
	using matchmaking_advertise = bool(*)(int localGamerIndex, uint32_t numSlots, uint32_t availableSlots, const rage::rlMatchingAttributes* attrs, uint64_t sessionId, const rage::rlSessionInfo* sessionInfo, rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status);
	using session_detail_response = bool(*)(void* /* rage::netTransactor */ _this, const void* /* rage::netTransactionInfo */ txInfo, rage::rlSessionDetailResponse* msg);
	using get_model_info = rage::fwArchetype* (*)(uint32_t key, uint32_t* return_index);
	using reset_network_complaints = void (*)(CNetComplaintMgr* mgr, bool force);
	using invalid_decal_crash = void(*)(uintptr_t c_ped, int component);
	using set_in_spectator_mode = void(*)(bool in_spectator_mode, int ped_index);
	using get_peer_by_security_id = rage::SecurityPeer*(*)(int id);
	using invalid_ped_component_draw = void(*)(rage::grmShaderGroup* a1, int a2, int64_t a3);
	using get_dlc_hash = int(*)(void* dlc_mgr, int seed);
	using get_searchlight = void* (*)(CPed*);
    using CExtraContentManager_AddContentFolder = bool(__fastcall*)(rage::CExtraContentManager*, const char* path);
	using CExtraContentManager_LoadContent = void(__fastcall*)(rage::CExtraContentManager*, bool executeChangeSet, bool executeEarlyStartup);

	run_script_threads g_run_script_threads{};
	handle_to_pointer g_handle_to_pointer{};
	pointer_to_handle g_pointer_to_handle{};
	// Bitbuffer Read/Write START
	read_bitbuf_dword g_read_bitbuf_dword{};
	read_bitbuf_string g_read_bitbuf_string{};
	read_bitbuf_bool g_read_bitbuf_bool{};
	read_bitbuf_array g_read_bitbuf_array{};
	write_bitbuf_qword g_write_bitbuf_qword{};
	write_bitbuf_dword g_write_bitbuf_dword{};
	write_bitbuf_int64 g_write_bitbuf_int64{};
	write_bitbuf_int32 g_write_bitbuf_int32{};
	write_bitbuf_bool g_write_bitbuf_bool{};
	write_bitbuf_array g_write_bitbuf_array{};
	// Bitbuffer Read/Write END
	c_task_jump_constructor g_c_task_jump_constructor{};
	c_task_fall_constructor g_c_task_fall_constructor{};
	run_async_module_request g_run_async_module_request{};
	has_interval_elapsed g_has_interval_elapsed{};
	dispatch_event g_dispatch_event{};
	script_vm g_script_vm{};
	sc_get_game_info_index g_sc_get_game_info_index{};
	CWeaponInfoManager* g_weapon_info_manager{};
	received_event g_received_event{};
	send_event_ack g_send_event_ack{};
	get_net_object g_get_net_object{};
	received_clone_remove g_received_clone_remove{};
	received_clone_sync g_receive_clone_sync{};
	receive_clone_create g_receive_clone_create{};
	can_apply_data g_can_apply_data{};
	get_sync_tree_for_type g_get_sync_tree_for_type{};
	get_entity_attached_to g_get_entity_attached_to{};
	get_host_array_handler_by_index g_get_host_array_handler_by_index{};
	get_title_caption_error_message_box g_get_title_caption_error_message_box{};
	connection_manager_try_free_memory g_connection_manager_try_free_memory{};
	remove_message_from_queue g_remove_message_from_queue{};
	remove_message_from_unacked_reliables g_remove_message_from_unacked_reliables{};
	get_scr_command_t g_get_scr_command{};
	get_screen_coords_from_world_coords g_get_screen_coords_from_world_coords{};
	get_ped_bone_coords g_get_ped_bone_coords{};
	get_gxt_label_from_table_t g_get_gxt_label_from_table{};
	get_joaated_gxt_label_from_table_t g_get_joaated_gxt_label_from_table{};
	request_control g_request_control{};
	clear_ped_task_network g_clear_ped_task_network{};
	migrate_object g_migrate_object{};
	ytd_file_register g_ytd_file_register{};
	get_native_handler g_get_native_handler{};
	write_player_game_state_data_node g_write_player_game_state_data_node{};
	trigger_script_event g_trigger_script_event{};
	get_gameplay_cam_coords g_get_gameplay_cam_coords{};
	get_final_rendered_cam_rot g_get_final_rendered_cam_rot{};
	send_invite_accepted_presence_event g_send_invite_accepted_presence_event{};
	give_pickup_rewards g_give_pickup_rewards{};
	get_model_info g_get_model_info{};
	reset_network_complaints g_reset_network_complaints{};
	invalid_decal_crash g_invalid_decal_crash{};
	set_in_spectator_mode g_set_in_spectator_mode{};
	remove_reference g_remove_reference{};
	start_get_session_by_gamer_handle g_start_get_session_by_gamer_handle{};
	show_profile_by_gamer_handle g_show_profile_by_gamer_handle{};
	send_friend_request g_send_friend_request{};
	invite_player_by_gamer_handle g_invite_player_by_gamer_handle{};
	join_session_by_info g_join_session_by_info{};
	get_connection_peer g_get_connection_peer{};
	handle_remove_gamer_cmd g_handle_remove_gamer_cmd{};
	format_i64_to_cash g_format_i64_to_cash{};
	get_blip_name_value g_get_blip_name_value{};
	matchmaking_advertise g_matchmaking_advertise{};
	session_detail_response g_session_detail_response{};
	cweapon_fire g_cweapon_fire{};
	queue_packet g_queue_packet{};
	send_packet g_send_packet{};
	get_peer_by_security_id g_get_peer_by_security_id{};
	invalid_ped_component_draw g_invalid_ped_component_draw{};
	get_dlc_hash g_get_dlc_hash{};
	get_searchlight g_get_searchlight{};
	CExtraContentManager_AddContentFolder g_CExtraContentManager_AddContentFolder{};
	CExtraContentManager_LoadContent g_CExtraContentManager_LoadContent{};
	rage::gameSkeleton* g_game_skeleton;
	void (*g_nullsub)();
	uint32_t* g_frame_count{};
	rage::atArray<GtaThread*>* g_script_threads{};
	PVOID g_assign_physical_index{};
	PVOID g_gta_thread_start{};
	PVOID g_gta_thread_kill{};
	PVOID g_get_label_text{};
	PVOID g_smpa_create_stub{};
	PVOID g_read_game_config{};
	PVOID g_get_pool_size{};
	PVOID g_create_pool{};
	PVOID g_get_pool_item{};
	PVOID g_allocate_memory_reliable{};
	PVOID g_world_model_spawn_bypass{};
	PVOID g_model_spawn_bypass{};
	PVOID g_http_start_request{};
	PVOID g_init_native_table{};
	PVOID g_matchmaking_unadvertise{};
	PVOID g_can_access_network_features{};
	PVOID g_network_bail{};
	bool* g_is_session_active{};
	void* g_gxt_labels{};
	void* g_jmp_rbx_register{};
	void* g_presecne_struct{};
	char** g_game_checksum_data{};
	void** g_dlc_manager{};
	rage::waypoint_data* g_waypoint_data{};
	rage::grcTextureStore* g_texture_store{};
	ScGameInfo* g_sc_game_info{};
	PVOID g_network_player_mgr_init{};
	PVOID g_network_player_mgr_shutdown{};
	FriendRegistry* g_friend_registry{};
	ScInfo* g_sc_info{};
	rage::CExtraContentManager** g_extra_content_manager{};
	CNetworkPlayerMgr** g_network_player_mgr{};
	CNetworkObjectMgr** g_network_object_mgr{};
	Network** g_network{};
	CPedFactory** g_ped_factory{};
	rage::scrProgramTable* g_scr_program_table{};
	rage::scrNativeRegistrationTable* g_native_registration_table{};
	IDXGISwapChain** g_swap_chain{};
	void** g_presence_data{};
	HashTable<CBaseModelInfo*>* g_hash_table{};
	rage::scrValue** g_globals{};
	eLoadingScreenState* g_loading_screen_state{};
	uint32_t* g_thread_id{};
	uint32_t* g_thread_count{};
	uint32_t* g_region_code{};
	uint32_t* g_object_ids_offset{};
	vehicle_pool*** g_vehicle_pool{};
	generic_pool** g_ped_pool{};
	generic_pool** g_prop_pool{};
	generic_pool** g_pickup_pool{};
	generic_pool** g_interior_proxy_pool{};
	generic_pool** g_vehicle_allocator{}; // this is not a normal pool
	const char* g_game_version{};
	const char* g_online_version{};
	PVOID g_task_parachute_object{};
	PVOID g_task_ambient_clips{};
	PVOID g_searchlight_crash;
	send_chat_message g_send_chat_message{};
	PVOID g_write_player_camera_data_node{};
	PVOID g_timecycle_keyframe_override{};
	PVOID g_write_vehicle_proximity_migration_data_node{};
	PVOID g_serialize_player_data_msg{};
	PVOID g_serialize_join_request_message{};
	PVOID g_serialize_take_off_ped_variation_task{};
	PVOID g_serialize_parachute_task{};
	PVOID g_fragment_physics_crash_2{};
	PVOID* g_draw_handler_mgr{};
	PVOID g_render_ped{};
	PVOID g_render_entity{};
	PVOID g_render_big_ped{};
	PVOID g_get_network_event_data{};
	PVOID g_error_message_box{};
	uint64_t g_network_config{};
	rage::atArray<CTrainConfig>* g_train_config_array{};
	PVOID g_received_array_update{};
	PVOID g_receive_pickup{};
	PVOID g_receive_net_message{};
	PVOID g_broadcast_net_array{};
	PVOID g_free_event_error{};
	PVOID g_activate_special_ability_patch{};
	PVOID g_report_myself_sender{};
	PVOID g_be_network_bail_patch{};
	bool* g_force_relay_connections{};

	HWND g_hwnd{};

	//BytePatches
	mem g_is_matchmaking_session_valid{};
	mem g_onlinecheck{};
	mem g_poolstuff{};
	mem g_morepoolstuff{};
	mem g_broadcast_patch{};
	mem g_creator_warp_cheat_triggered_patch{};
	mem g_crash_trigger{};
	mem g_max_wanted_level{};
	mem g_blame_explode{};
	mem g_explosion_patch{};
	mem g_script_vm_patch_1{};
	mem g_script_vm_patch_2{};
	mem g_script_vm_patch_3{};
	mem g_script_vm_patch_4{};
	mem g_script_vm_patch_5{};
	mem g_script_vm_patch_6{};

	get_net_game_player g_net_game_player;
	uint64_t m_ui_3d_draw_manager;
	uint64_t m_push_scene_present_to_manager;
	uint64_t m_add_element_to_scene;
	uint64_t m_set_scene_element_lightning;
};

inline std::unique_ptr<pointers> g_pointers{};

// everything pasted from https://github.com/gta-chaos-mod/ChaosModV/blob/master/ChaosMod/Util/EntityIterator.h
// Thanks to menyoo for most of these!!

// Pool Interator class to iterate over pools. Has just enough operators defined to be able to be used in a for loop, not suitable for any other iterating.

template<typename T>
class pool_iterator
{
public:
	T* pool = nullptr;
	uint32_t index = 0;

	explicit pool_iterator(T* pool, int32_t index = 0)
	{
		this->pool = pool;
		this->index = index;
	}

	pool_iterator& operator++()
	{
		for (index++; index < pool->m_size; index++)
		{
			if (pool->is_valid(index))
			{
				return *this;
			}
		}

		index = pool->m_size;
		return *this;
	}

	rage::CEntity* operator*()
	{
		auto address = pool->get_address(index);
		return reinterpret_cast<rage::CEntity*>(address);
	}

	bool operator!=(const pool_iterator& other) const
	{
		return this->index != other.index;
	}
};

// Common functions for VehiclePool and GenericPool
template<typename T>
class pool_utils
{
public:
	auto to_array()
	{
		std::vector<Entity> arr;
		for (auto entity : *static_cast<T*>(this))
		{
			if (entity)
				arr.push_back(g_pointers->g_pointer_to_handle(entity));
		}

		return arr;
	}

	auto begin()
	{
		return ++pool_iterator<T>(static_cast<T*>(this), -1);
	}

	auto end()
	{
		return ++pool_iterator<T>(static_cast<T*>(this), static_cast<T*>(this)->m_size);
	}
};

class vehicle_pool : public pool_utils<vehicle_pool>
{
public:
	UINT64* m_pool_address;
	UINT32 m_size;
	char _Padding2[36];
	UINT32* m_bit_array;
	char _Padding3[40];
	UINT32 m_item_count;

	bool is_valid(UINT32 i) const
	{
		return m_bit_array[i >> 5] >> (i & 0x1F) & 1;
	}

	UINT64 get_address(UINT32 i) const
	{
		return m_pool_address[i];
	}
};

class generic_pool : public pool_utils<generic_pool>
{
public:
	UINT64 m_pool_address; // 0x0
	BYTE* m_bit_array;     // 0x8
	UINT32 m_size;         // 0x10
	UINT32 m_item_size;    // 0x14
	UINT32 m_pad[2];       // 0x18
	UINT32 m_item_count;   // 0x20

	bool is_valid(const UINT32 i) const
	{
		return mask(i) != 0;
	}

	UINT64 get_address(const UINT32 i) const
	{
		return mask(i) & m_pool_address + static_cast<uint64_t>(i) * m_item_size;
	}

	int get_item_count() const
	{
		return 4 * static_cast<int>(m_item_count) >> 2;
	}

private:
	int64_t mask(const UINT32 i) const
	{
		int64_t num1 = m_bit_array[i] & 0x80;
		return ~((num1 | -num1) >> 63);
	}
};
static_assert(offsetof(generic_pool, generic_pool::m_item_count) == 0x20);