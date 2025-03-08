#pragma once
#include "functions/input.h"
#include "script/home/home.h"
#include "types/submenu.hpp"

namespace ui
{
	inline submenu* g_current_menu = &submenus::g_home_submenu;

	namespace menu
	{
		void push(const submenu* sub);
		void pop();
	}

	namespace handlers
	{
		namespace sounds
		{
			inline soundData g_open{ true, "SELECT" };
			inline soundData g_close{ true, "BACK" };
			inline soundData g_enter{ true, "SELECT" };
			inline soundData g_back{ true, "BACK" };
			inline soundData g_left{ true, "NAV_LEFT_RIGHT" };
			inline soundData g_right{ true, "NAV_LEFT_RIGHT" };
			inline soundData g_up{ true, "NAV_UP_DOWN" };
			inline soundData g_down{ true, "NAV_UP_DOWN" };
			extern void queue(const soundData& sound);
		}

		namespace key
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

			void actions();
		}
	}
}
