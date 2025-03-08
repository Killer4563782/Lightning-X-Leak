#pragma once
#include "memory/pattern.h"

class script_function
{
	rage::joaat_t m_script;
	const memory::pattern m_pattern;
	int32_t m_offset;
	int32_t m_ip;
	std::string m_name;
public:
	script_function(const std::string& name, rage::joaat_t script, const std::string& pattern, int32_t offset);
	void populate_ip();
	void call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args);
	void call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args, bool& done);
	void static_call(std::initializer_list<uint64_t> args);
	void operator()(std::initializer_list<uint64_t> args);
};

namespace scr_functions
{
	static inline script_function set_freemode_session_active("SFSA", "freemode"_joaat, "2D 00 02 00 00 75 5D ? ? ? 50", 0);
	static inline script_function reset_session_data("RSD", "pausemenu_multiplayer"_joaat, "2D 02 7D 00 00", 0);
}