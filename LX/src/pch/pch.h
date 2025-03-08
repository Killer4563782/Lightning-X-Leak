#pragma once
#include "framework.h"
#include "macros.h"
#include "types.h"

#include "ragepch.hpp"
#include "rage/lists.hpp"

#define BRAND "Lightning X"
#define UPPER_BRAND "LIGHTNING X"
#define UPPER_BRAND_DASH "LIGHTNING-X"
#define LOWER_BRAND "lightning x"
#define LOWER_BRAND_DASH "lightning-x"
#define SHORT_UPPER_BRAND "LX"
#define SHORT_LOWER_BRAND "lx"
										
#define DISCORD_LARGE_IMAGE_KEY "https://isniffsharpie.com/Icons/LX_LOGO.gif"
#define MENU_VERSION "6.1.6.R3"
#define SUPPORTED_VERSION "3442"

inline bool g_running = true;
inline bool g_open = false;
inline HMODULE g_module{};
inline HANDLE g_thread{};

inline fs::path g_lx_path{};		// AppData/Roaming/Lightning X (BRAND)
inline fs::path g_module_path{};	// Some injectors rename / move the dll

namespace self
{
	inline CNetGamePlayer* local_player{};
	inline CPed* local_ped{};
	inline Vector3 position{};
	inline Ped ped{};
	inline Player player{};
	inline Vehicle vehicle{};
	inline std::set<int> spawned_vehicles{};
}
