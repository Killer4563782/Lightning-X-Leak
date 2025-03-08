#pragma once
#include "fiber/script.hpp"
#include "memory/pointers.h"
#include "rage/util.h"
#include "rage/gta/net_object_mgr.hpp"
#include "services/online/online.h"

namespace util
{
	static void wait_for_splash_screen_end()
	{
		if (const auto& state = *g_pointers->g_loading_screen_state; state == eLoadingScreenState::INTRO_MOVIE || state == eLoadingScreenState::LEGALMAIN || state == eLoadingScreenState::LEGALSPLASH)
		{
			LOG(info, "Waiting for splash screen to finish...");
			while (state == eLoadingScreenState::INTRO_MOVIE || state == eLoadingScreenState::LEGALMAIN || state == eLoadingScreenState::LEGALSPLASH)
			{
				std::this_thread::sleep_for(100ms);
			}
		}
	}

	static void wait_for_game_load()
	{
		if (const auto& state = *g_pointers->g_loading_screen_state; state != eLoadingScreenState::NONE)
		{
			LOG(info, "Waiting for Game to Load...");
			while (state != eLoadingScreenState::NONE)
			{
				std::this_thread::sleep_for(100ms);
			}
		}
	}

	static constexpr std::uint32_t rp_model_list[] = {
	    0x4D6514A3,
	    0x1A9736DA,
	    0x748F3A2A,
	    0x3D1B7A2F,
	    0x1A126315,
	    0xD937A5E9,
	    0x23DDE6DB,
	    0x991F8C36,
	    0xB4A24065,
	};

	namespace network
	{
		namespace session
		{
			inline rage::snSession* get()
			{
				return get_network()->m_game_session_ptr;
			}

			inline rage::snSession* get_transition()
			{
				return get_network()->m_transition_session_ptr;
			}

			inline rage::CDynamicEntity* get_entity_from_sguid(const Entity sguid)
			{
				return g_pointers->g_handle_to_pointer(sguid);
			}

			inline Entity get_sguid_from_entity(const rage::CEntity* entity)
			{
				Entity sguid = -1;
				for (const auto container = entity->m_extension_container; container; container->m_next)
				{
					if (rage::fwExtension* entry = container->m_entry)
					{
						if (const int32_t id = static_cast<int32_t>(entry->get_id()); id && (id <= sguid || sguid == -1))
						{
							sguid = id;
						}
					}
				}
				return sguid;
			}

			namespace peer
			{
				inline rage::snPeer* get_via_connection_id(const uint8_t con_id)
				{
					return get()->m_peers[con_id];
				}
			}
		}

		inline bool is_host(uint8_t index)
		{
			auto player = (*g_pointers->g_network_player_mgr)->m_player_list[index];
			return player && player->is_valid() && player->is_host();
		}

		inline CNetGamePlayer* get_script_host_net_game_player()
		{
			if (auto thr = find_script_thread("freemode"_joaat); thr && thr->m_net_component)
				return reinterpret_cast<CGameScriptHandlerNetComponent*>(thr->m_net_component)->get_host();

			return nullptr;
		}
	}

	namespace natives
	{
		inline bool request_weapon_asset(const Hash& hash)
		{
			if (WEAPON::HAS_WEAPON_ASSET_LOADED(hash))
			{
				return true;
			}

			if (WEAPON::IS_WEAPON_VALID(hash))
			{
				while (!WEAPON::HAS_WEAPON_ASSET_LOADED(hash))
				{
					WEAPON::REQUEST_WEAPON_ASSET(hash, 31, 0);
					script::get_current()->yield();
				}

				return true;
			}

			return false;
		}

		inline bool request_named_ptfx_asset(const char* name)
		{
			if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(name))
			{
				return true;
			}

			bool has_loaded;
			do
			{
				has_loaded = STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(name);

				if (has_loaded)
					return true;

				STREAMING::REQUEST_NAMED_PTFX_ASSET(name);

				script::get_current()->yield();
			} while (!has_loaded);

			return false;
		}

		inline bool request_model(rage::joaat_t hash, bool skip_check = false)
		{
			if (STREAMING::HAS_MODEL_LOADED(hash)) 
			{
				return true;
			}

			if (skip_check || (STREAMING::IS_MODEL_VALID(hash) && STREAMING::IS_MODEL_IN_CDIMAGE(hash)))
			{
				bool has_loaded;
				do
				{
					has_loaded = STREAMING::HAS_MODEL_LOADED(hash);

					if (has_loaded)
						break;

					STREAMING::REQUEST_MODEL(hash);

					script::get_current()->yield();
				} while (!has_loaded);

				return true;
			}

			return false;
		}

		inline bool network_has_control_of_entity(rage::netObject* net_object)
		{
			return !net_object || !net_object->m_is_remote && net_object->m_wants_to_be_owner == -1;
		}

		inline bool request_control(Entity ent, int timeout = 420)
		{
			if (!services::is_online())
				return true;

			const auto hnd = g_pointers->g_handle_to_pointer(ent);
			if (!hnd || !hnd->m_net_object || !services::is_online())
				return false;

			if (network_has_control_of_entity(hnd->m_net_object))
				return true;

			for (int i = 0; i < timeout; i++)
			{
				g_pointers->g_request_control(hnd->m_net_object);
				if (network_has_control_of_entity(hnd->m_net_object))
					return true;

				if (timeout > 0)
					script::get_current()->yield();
			}
			return false;
		}

		inline bool forcefully_take_control(Entity ent)
		{
			if (!services::is_online() || request_control(ent))
			{
				return true;
			}
			
			rage::CDynamicEntity* entity = network::session::get_entity_from_sguid(ent);
			rage::netObject* net_obj = entity->m_net_object;
			auto obj_mgr = *g_pointers->g_network_object_mgr;
			obj_mgr->ChangeOwner(net_obj, self::local_player, 0);
			return true;
		}
	}

	namespace caller
	{
		template<typename T, typename ...Args>
		static T call(uint64_t address, Args... args) { return reinterpret_cast<T(*)(Args...)>(address)(args...); }
	}

	inline void async(const std::function<void()>& callback)
	{
		std::thread(callback).detach();
	}

	inline void delayed_thread(const bool& running, milliseconds ms, const std::function<void()>& callback)
	{
		async([&]
		{
			while (running)
			{
				if (callback)
					callback();
				std::this_thread::sleep_for(ms);
			}
		});
	}

	inline bool in_module_region(const char* module, uint64_t address)
	{
		static HMODULE hmod{ GetModuleHandleA(module ? module : nullptr) };
		static uint64_t module_base{};
		static uint64_t module_size{};
		if (!module_base || !module_size)
		{
			MODULEINFO info{};
			if (!K32GetModuleInformation(GetCurrentProcess(), hmod, &info, sizeof(info)))
			{
				return true;
			}
			module_base = reinterpret_cast<uint64_t>(hmod);
			module_size = static_cast<uint64_t>(info.SizeOfImage);
		}
		return address > module_base && address < module_base + module_size;
	}

	inline bool check_ins(const char* module, uint64_t address, uint8_t ins)
	{
		if (!in_module_region(module, address))
		{
			return false;
		}
		return *reinterpret_cast<uint8_t*>(address) == ins;
	}

	inline void iterator_files_in_path(const fs::path& path, const std::string& ext, const std::function<void(fs::path, const std::string&)>& cb)
	{
		if (exists(path))
		{
			for (fs::directory_iterator iterator{ path.string() }; auto && entry : iterator)
			{
				if (entry.is_regular_file())
				{
					if (const fs::path& paths{ entry.path() }; paths.has_filename() && paths.extension() == ext)
					{
						cb(paths, paths.filename().string());
					}
				}
			}
		}
	}

	inline std::string time(const std::string& format)
	{
		char time_buf[256];
		int64_t time_since_epoch = std::time(nullptr);
		tm localtime;
		localtime_s(&localtime, &time_since_epoch);
		strftime(time_buf, sizeof(time_buf), format.c_str(), &localtime);
		return time_buf;
	}

	enum class eBlipHandleType : int8_t
	{
		closest,
		first,
		next
	};

	class blip
	{
	public:
		blip(int8_t sprite, eBlipHandleType handle_type) : m_sprite(sprite)
		{
			switch (handle_type)
			{
			case eBlipHandleType::closest:
			{
				m_handle = HUD::GET_CLOSEST_BLIP_INFO_ID(m_sprite);
			}
			break;
			case eBlipHandleType::first:
			{
				m_handle = HUD::GET_FIRST_BLIP_INFO_ID(m_sprite);
			}
			break;
			case eBlipHandleType::next:
			{
				m_handle = HUD::GET_NEXT_BLIP_INFO_ID(m_sprite);
			}
			break;
			}
			m_type = HUD::GET_BLIP_INFO_ID_TYPE(handle());
			coords() = HUD::GET_BLIP_COORDS(handle());
			alpha() = HUD::GET_BLIP_ALPHA(handle());
			rotation() = HUD::GET_BLIP_ROTATION(handle());
			color() = HUD::GET_BLIP_COLOUR(handle());
			hud_color() = HUD::GET_BLIP_HUD_COLOUR(handle());
		}

		void set_blip_coords(const Vector3& value)
		{
			coords() = value;
			HUD::SET_BLIP_COORDS(handle(), coords().x, coords().y, coords().z);
		}

		void set_alpha(int32_t value)
		{
			alpha() = value;
			HUD::SET_BLIP_ALPHA(handle(), alpha());
		}

		void set_scale(float value)
		{
			scale() = value;
			HUD::SET_BLIP_SCALE(handle(), scale());
		}

		void set_color(int32_t value)
		{
			color() = value;
			HUD::SET_BLIP_COLOUR(handle(), color());
		}

		void remove()
		{
			HUD::REMOVE_BLIP(&handle());
		}

		bool on_minimap()
		{
			return HUD::IS_BLIP_ON_MINIMAP(handle());
		}

		bool exists()
		{
			return HUD::DOES_BLIP_EXIST(handle());
		}

		Blip& handle()
		{
			return m_handle;
		}

		Vector3& coords()
		{
			return m_coords;
		}

		int32_t& alpha()
		{
			return m_alpha;
		}

		float& scale()
		{
			return m_scale_;
		}

		int32_t& rotation()
		{
			return m_rotation_;
		}

		int32_t& color()
		{
			return m_color;
		}

		int32_t& hud_color()
		{
			return m_hud_color;
		}

	private:
		Blip m_handle{};
		int8_t m_sprite{};
		int32_t m_type{};
		Vector3 m_coords{};
		int32_t m_alpha{};
		float m_scale_{};
		int32_t m_rotation_{};
		int32_t m_color{};
		int32_t m_hud_color{};
	};
}
