#pragma once
#include "helpers/folderHelpers.h"
#include "memory/pointers.h"
#include "rage/gameSkeleton.hpp"

namespace core::util
{
	inline void initialize_lx_path_folder()
	{
		if (TCHAR module_path[MAX_PATH]; GetModuleFileName(g_module, module_path, MAX_PATH) > NULL)
		{
			g_module_path = module_path;
		}
		else
		{
			throw std::runtime_error("Failed to get module path.");
		}

		g_lx_path = folder_helpers::get_folder_path(&FOLDERID_RoamingAppData);
		g_lx_path /= BRAND;

		if (g_lx_path.empty())
		{
			throw std::runtime_error("Failed to get LX path | Please Restart LXLauncher to install all required files!");
		}

		for (const std::array<fs::path, 5> sub_dirs = { "Fonts", "Headers", "CustomCars", "Scripts" "CustomOutfit"}; const auto & sub_dir : sub_dirs)
		{
			if (fs::path full_path = g_lx_path / sub_dir; !exists(full_path))
			{
				create_directories(full_path);
			}
		}
	}

	inline bool supported_game_version()
	{
		if (!g_running)
		{
			return false;
		}
		
		if (!g_pointers->g_game_version || std::strcmp(g_pointers->g_game_version, SUPPORTED_VERSION) != 0)
		{
			LOG_DEBUG("Unsupported game version: {} / {}", g_pointers->g_game_version, SUPPORTED_VERSION);
			if (MessageBoxA(nullptr, "You are using an unsupported game version\nDo you still want to continue?", BRAND, MB_YESNO | MB_ICONERROR) == IDNO)
			{
				return false;
			}
		}

		return true;
	}


	inline constexpr std::array<const uint32_t, 5> g_bad_hashes
	{
		"fwClothMeshing"_joaat, // rageSecEngine
		"CCreditsText"_joaat, // rageSecGamePluginManager
		"rageSecEngine"_joaat,
		"rageSecGamePluginManager"_joaat,
		"TamperActions"_joaat
	};

	inline bool known_bad_hash(uint32_t hash)
	{
		return std::ranges::any_of(g_bad_hashes, [hash](const uint32_t& h) { return hash == h; });
	}

	inline bool patch_init()
	{
		bool patched = false;
		rage::gameSkeleton& skeleton = *g_pointers->g_game_skeleton;
		if (const rage::gameSkeleton::mode* the_mode = skeleton.GetMode(skeleton.m_InitModes, 1))
		{
			const rage::gameSkeleton::dependency* curr_dep = the_mode->head;
			while (curr_dep)
			{
				if (int num_systems = curr_dep->sysData.count(); num_systems > 0)
				{
					for (int system_index = 0; system_index < num_systems; system_index++)
					{
						if (int index = system_index; curr_dep->sysData[index] < static_cast<unsigned>(skeleton.m_RegisteredSystemData.count()))
						{
							if (rage::gameSkeleton::systemData& sys_data = skeleton.m_RegisteredSystemData[curr_dep->sysData[index]]; known_bad_hash(sys_data.m_SystemName))
							{
								sys_data.m_InitFunction = reinterpret_cast<rage::gameSkeleton::fnInitFunction>(g_pointers->g_nullsub);
								sys_data.m_ShutdownFunction = reinterpret_cast<rage::gameSkeleton::fnShutdownFunction>(g_pointers->g_nullsub);
								patched = true;
							}
						}
					}
				}
				curr_dep = curr_dep->next;
			}
		}
		return patched;
	}

	inline bool patch_update()
	{
		bool patched = false;
		rage::gameSkeleton& skeleton = *g_pointers->g_game_skeleton;
		for (rage::gameSkeleton::updateMode* mode = skeleton.m_UpdateModes; mode; mode = mode->m_Next)
		{
			for (rage::gameSkeleton::updateBase* update_node = mode->m_Head; update_node; update_node = update_node->m_Next)
			{
				if (update_node->m_Name != "Common Main"_joaat)
					continue;
				auto group = static_cast<rage::gameSkeleton::updateGroup*>(update_node);
				for (rage::gameSkeleton::updateBase* group_child_node = group->m_Head; group_child_node; group_child_node = group_child_node->m_Next)
				{
					if (group_child_node->m_Name != 0xA0F39FB6 && group_child_node->m_Name != "TamperActions"_joaat)
						continue;
					patched = true;
					reinterpret_cast<rage::gameSkeleton::updateElement*>(group_child_node)->m_UpdateFunction = g_pointers->g_nullsub;
				}
				break;
			}
		}
		return patched;
	}
	inline bool disable_rage_sec()
	{
		// vali: The J
		//TRY_CATCH({ patched = patch_init(); });
		bool patched = patch_update();
		return patched;
	}
}
