
#include "scripts.h"

#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "scripting/scripting.h"

namespace ui::submenus
{
	void scripts_submenu::on_tick()
	{
		m_options.clear();

		this->add(function_option("Reload Scripts", "Reloads all scripts in the Scripts folder", [this]
		{
			g_scripting->m_cached_scripts.clear();
			g_scripting->cache_scripts();
			this->on_enter();
		}));

		if (auto scripts = g_scripting->m_cached_scripts; !scripts.empty())
		{
			this->add(break_option("Scripts"));
			for (const auto& script : scripts)
			{
				this->add(function_option(std::format("{} [{}]", script.m_name, script.m_loaded ? "Loaded" : "Unloaded").c_str(), {}, [script]
				{
					if (!exists(g_lx_path / "Scripts" / script.m_name))
					{
						return notify(Error, "The script file does not exist anymore");
					}

					g_scripting->execute_script(script.m_name);
				}));
			}
		}
	}
}
