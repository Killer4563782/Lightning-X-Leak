#pragma once
#include "script.hpp"

using script_list = std::vector<std::unique_ptr<script>>;

class script_mgr
{
public:
	explicit script_mgr() = default;
	~script_mgr() = default;

	void add_script(std::unique_ptr<script> script);
	void remove_all_scripts();
	

	void for_each_script(auto func);

	void tick();

	bool can_tick() const
	{
		return m_can_tick;
	}

private:
	void ensure_main_fiber();
	void tick_internal();

	std::recursive_mutex m_mutex;
	script_list m_scripts;

	bool m_can_tick = false;
};

inline script_mgr g_script_mgr;
