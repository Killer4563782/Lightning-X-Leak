#pragma once
#include "memory/pointers.h"

namespace gxt
{
	inline std::optional<std::string> get_gxt_label(const std::string& key, bool return_key = false)
	{
		const auto gxt_via_string = g_pointers->g_get_gxt_label_from_table(g_pointers->g_gxt_labels, key.c_str());
		if (auto str = std::string(gxt_via_string); !str.empty())
			return str;

		const auto gxt_via_joaat = g_pointers->g_get_joaated_gxt_label_from_table(g_pointers->g_gxt_labels, rage::joaat(key));
		if (auto str = std::string(gxt_via_joaat); !str.empty())
			return str;

		if (return_key)
			return key;

		return std::nullopt;
	}
}
