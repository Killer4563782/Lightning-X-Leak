#pragma once
#include "methods/detour_hook.hpp"
#include "methods/vmt_hook.hpp"
#include "minhook/MinHook.h"
#include <rage/gta/net_game_event.hpp>

inline uint32_t g_LastPoolHash{};
inline std::unordered_map<void*, uint32_t> g_Pools{};


inline CNetGamePlayer* g_syncing_player = nullptr;
inline eNetObjType g_syncing_object_type = static_cast<eNetObjType>(-1);

struct hooks
{
	static bool run_script_threads(uint32_t ops_to_execute);

	static constexpr auto swapchain_num_funcs = 19;
	static constexpr auto swapchain_present_index = 8;
	static constexpr auto swapchain_resizebuffers_index = 13;
	static HRESULT present(IDXGISwapChain* this_, UINT sync_interval, UINT flags);
	static HRESULT resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags);

	static LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static const char* get_label_text(void* unk, const char* text_code);

	static GtaThread* gta_thread_start(unsigned int** a1, unsigned int a2);
	static rage::eThreadState gta_thread_kill(GtaThread* thread);
	static bool init_native_tables(rage::scrProgram* program);
	static rage::eThreadState script_vm(rage::scrValue* start_stack, rage::scrValue** scr_globals, rage::scrProgram* program, rage::scrThreadContext* ctx);

	static bool network_player_mgr_init(CNetworkPlayerMgr* _this, uint64_t a2, uint32_t a3, uint32_t a4[4]);
	static void network_player_mgr_shutdown(CNetworkPlayerMgr* _this);

	static bool fragment_physics_crash_2(__m128* object0, __m128* object1);

	static void received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int unk, rage::datBitBuffer* bit_buffer);

	// these two aren't actually hooks, just helper functions for hooks
	static bool increment_stat_event(const CNetworkIncrementStatEvent* net_event, CNetGamePlayer* sender);
	static bool scripted_game_event(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player);
	static int  get_battle_eye_init_state();
	static uint32_t can_access_network_features(eNetworkAccessArea accessArea, eNetworkAccessCode* nAccessCode);

	static bool receive_net_message(void* a1, rage::netConnectionManager* net_cxn_mgr, rage::netEvent* event);
	static rage::CEventNetwork* get_network_event_data(int64_t unk, rage::CEventNetwork* net_event);
	static rage::fwConfigManagerImpl<CGameConfig>* ReadGameConfig(rage::fwConfigManagerImpl<CGameConfig>* manager, const char* file);
	static void* GetPoolItem(void* pool);
	/*static unsigned int GetPoolSize(rage::fwConfigManagerImpl<CGameConfig>* mgr, uint32_t hash, int defaultValue);*/

	static void* assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index);

	//SYNC
	static void received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp);
	static NetObjectAckCode received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, uint16_t object_id, rage::datBitBuffer* bufer, uint16_t unk, uint32_t timestamp);
	static bool can_apply_data(rage::netSyncTree* tree, rage::netObject* object);
	static void searchlight_crash(void* a1, CPed* ped);

	static void write_player_game_state_data_node(rage::netObject* player, CPlayerGameStateDataNode* node);
	static int get_dlc_hash(void* mgr, int seed);

	static void invalid_decal(uintptr_t c_ped, int component);
	static void invalid_ped_component_draw(rage::grmShaderGroup* a1, int a2, int64_t a3);
	static int task_parachute_object(uint64_t _this, int a2, int a3);
	static int task_ambient_clips(uint64_t _this, int a2, int a3);

	static bool handle_join_request(Network* network, rage::snSession* session, rage::rlGamerInfo* player_info, CJoinRequestContext* ctx, BOOL is_transition_session);


	static bool send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team);


	static bool serialize_player_data_msg(CNetGamePlayerDataMsg* msg, rage::datBitBuffer* buffer);
	static bool serialize_join_request_message(RemoteGamerInfoMsg* info, void* data, int size, int* bits_serialized);


	static unsigned int broadcast_net_array(rage::netArrayHandlerBase* _this, CNetGamePlayer* target, rage::datBitBuffer* bit_buffer, uint16_t counter, uint32_t* elem_start, bool silent);


	static void serialize_take_off_ped_variation_task(ClonedTakeOffPedVariationInfo* info, rage::CSyncDataBase* serializer);
	static void serialize_parachute_task(__int64 info, rage::CSyncDataBase* serializer);

	static int nt_query_virtual_memory(void* _this, HANDLE handle, PVOID base_addr, int info_class, MEMORY_BASIC_INFORMATION* info, int size, size_t* return_len);

	static bool http_start_request(const char* uri);

	static bool received_array_update(rage::netArrayHandlerBase* array, CNetGamePlayer* sender, rage::datBitBuffer* buffer, int size, int16_t cycle);

	static bool receive_pickup(rage::netObject* netobject, void* unk, CPed* ped);

	static void write_player_camera_data_node(rage::netObject* player, CPlayerCameraDataNode* node);


	static int64_t task_jump_constructor(uint64_t a1, int a2);

	static void* task_fall_constructor(uint64_t _this, int flags);

	static void write_vehicle_proximity_migration_data_node(rage::netObject* veh, CVehicleProximityMigrationDataNode* node);

	static void log_error_message_box(rage::joaat_t joaated_error_code, bool a2);

	static void update_timecycle_keyframe_data(int64_t timecycleManager, TimecycleKeyframeData* timecycleKeyframeData);
	static void* SMPACreateStub(void* a1, void* a2, size_t size, void* a4, bool a5);
	static void* CreatePool(void* pool, int size, const char* name, int unk1, int unk2, bool unk3);
	static void* allocate_memory_reliable(rage::netConnection* cxn, int required_memory);

	static void* render_ped(__int64 renderer, CPed* ped, __int64 a3, __int64 a4);
	static void render_entity(__int64 renderer, rage::fwEntity* entity, int unk, bool a4);
	static __int64 render_big_ped(__int64 renderer, CPed* ped, __int64 a3, __int64 a4);

	static void received_clone_remove(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, int16_t object_id, uint32_t ownership_token);

	static void format_int64_to_cash(int64_t cash, char* cash_string, int cash_string_size, bool use_commas);

	static char* player_tags_on_minimap(__int64 blip);
	static bool rage_netTransactor_SendResponse_rage_rlSessionDetailResponse(void* /* rage::netTransactor */ _this, const void* /* rage::netTransactionInfo */ txInfo, rage::rlSessionDetailResponse* msg);
	static bool rage_rlScMatchmaking_Advertise(int localGamerIndex, uint32_t numSlots, uint32_t availableSlots, const rage::rlMatchingAttributes* attrs, uint64_t sessionId, const rage::rlSessionInfo* sessionInfo, rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status);
	static bool rage_rlScMatchmaking_Unadvertise(int localGamerIndex, const rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status);
	static bool c_weapon_fire(__int64 _this, __int64 params);
};

class minhook_keepalive
{
public:
	minhook_keepalive()
	{
		MH_Initialize();
	}
	~minhook_keepalive()
	{
		MH_Uninitialize();
	}
};

class hooking
{
	friend hooks;

public:
	explicit hooking();
	~hooking();

	void enable();
	void disable();

	class detour_hook_helper
	{
		friend hooking;

		using ret_ptr_fn = std::function<void*()>;

		ret_ptr_fn m_on_hooking_available = nullptr;

		detour_hook* m_detour_hook;

		void enable_hook_if_hooking_is_already_running() const;

		template<auto DetourFunction>
		struct hook_to_detour_hook_helper
		{
			static inline detour_hook m_detour_hook;
		};
	public:
		template<auto DetourFunction>
		static void add(const std::string& name, void* target)
		{
			hook_to_detour_hook_helper<DetourFunction>::m_detour_hook.set_instance(name, target, DetourFunction);

			detour_hook_helper d{};
			d.m_detour_hook = &hook_to_detour_hook_helper<DetourFunction>::m_detour_hook;

			d.enable_hook_if_hooking_is_already_running();

			m_detour_hook_helpers.push_back(d);
		}

		template<auto DetourFunction>
		static void* add_lazy(const std::string& name, const ret_ptr_fn on_hooking_available)
		{
			hook_to_detour_hook_helper<DetourFunction>::m_detour_hook.set_instance(name, DetourFunction);

			detour_hook_helper d{};
			d.m_detour_hook = &hook_to_detour_hook_helper<DetourFunction>::m_detour_hook;
			d.m_on_hooking_available = on_hooking_available;

			d.enable_hook_if_hooking_is_already_running();

			m_detour_hook_helpers.push_back(d);

			return nullptr;
		}

		~detour_hook_helper();
	};

	template<auto DetourFunction>
	static auto get_original()
	{
		return detour_hook_helper::hook_to_detour_hook_helper<DetourFunction>::m_detour_hook.template get_original<decltype(DetourFunction)>();
	}

private:
	bool m_enabled{};
	minhook_keepalive m_minhook_keepalive;

	vmt_hook* m_swapchain_hook;

	WNDPROC m_og_wndproc = nullptr;

	static inline std::vector<detour_hook_helper> m_detour_hook_helpers;
	
};

inline std::unique_ptr<hooking> g_hooking{};

static inline std::vector<rage::rlScMatchmakingMatchId> session_advertisements{};
