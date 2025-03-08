#pragma once
#include "gui/types/submenu.hpp"
#include "services/reactions/reactions.h"

namespace ui::submenus
{
	class protection_submenu final : public submenu
	{
	public:
		Reaction* m_reaction;
		bool m_allow_block;

		protection_submenu(std::string name, std::string description, Reaction* reaction, bool allow_block = true);

		std::string get_tags() const;
		void draw(bool selected) override;
		void on_init() override;
	};


	class protections_submenu final : public submenu
	{
		bool m_toggle_all_state = true;

		using submenu::submenu;

		void on_init() override;
	};

	extern protections_submenu g_protections_submenu;
}
