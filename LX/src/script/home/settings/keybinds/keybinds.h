#pragma once
#include "functions/input.h"
#include "gui/types/submenu.hpp"

namespace ui::handlers::key
{
	extern Input open;
	extern Input enter;
	extern Input back;

	extern Input up;
	extern Input down;
	extern Input left;
	extern Input right;

	extern Input move_up;
	extern Input move_down;
	extern Input move_forward;
	extern Input move_backward;
	extern Input move_left;
	extern Input move_right;
}

namespace ui::submenus
{
	class keybind_submenu final : public submenu
	{
	public:
		Input* m_input;
		std::string m_original_description;
		bool m_allow_disable_repeat;

		keybind_submenu(std::string name, std::string description, Input* input, bool allow_disable_repeat = true);

		std::string get_description() const;

		void on_tick() override;
	};


	class keybinds_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}
