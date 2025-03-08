#include "interface.h"

#include "fiber/fiber_pool.hpp"
#include "script/home/home.h"

namespace ui
{
	namespace menu
	{
		void push(submenu* sub)
		{
			g_current_menu = sub;
		}

		void pop()
		{
			if (g_current_menu->m_parent)
			{
				g_current_menu = g_current_menu->m_parent;
			}
			else
			{
				g_open = false;
				g_current_menu = &submenus::g_home_submenu;
			}
		}
	}

	namespace handlers
	{
		namespace sounds
		{
			void queue(const soundData& sound)
			{
				if (sound.m_active)
				{
					g_fiber_pool->queue_job([&]
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, sound.m_id.data(), sound.m_set.data(), NULL);
					});
				}
			}
		}

		namespace key
		{
			Input open{ ImGuiKey_Insert, ImGuiKey_F5, ImGuiKey_GamepadDpadRight, ImGuiKey_GamepadR1, true, false, true, true };
			Input enter{ ImGuiKey_Enter, ImGuiKey_GamepadFaceDown };
			Input back{ ImGuiKey_Backspace, ImGuiKey_None, ImGuiKey_GamepadFaceRight, ImGuiKey_None, true, false, false, true }; 

			Input up{ ImGuiKey_UpArrow, ImGuiKey_None, ImGuiKey_GamepadDpadUp, ImGuiKey_None, 180.f, 50.f, false, false };
			Input down{ ImGuiKey_DownArrow, ImGuiKey_None, ImGuiKey_GamepadDpadDown, ImGuiKey_None, 180.f, 50.f, false, false };
			Input left{ ImGuiKey_LeftArrow, ImGuiKey_GamepadDpadLeft };
			Input right{ ImGuiKey_RightArrow, ImGuiKey_GamepadDpadRight };

			// Freecam / No Clip
			Input move_up{ ImGuiKey_LeftShift, ImGuiKey_None, ImGuiKey_GamepadDpadUp, ImGuiKey_None, true, false, false, false, 0.f, 0.f };
			Input move_down{ ImGuiKey_LeftCtrl, ImGuiKey_None, ImGuiKey_GamepadDpadDown, ImGuiKey_None, true, false, false, false, 0.f, 0.f };
			Input move_forward{ ImGuiKey_W, ImGuiKey_None, ImGuiKey_GamepadLStickUp, ImGuiKey_None, true, false, false, false, 0.f, 0.f };
			Input move_backward{ ImGuiKey_S, ImGuiKey_None, ImGuiKey_GamepadLStickDown, ImGuiKey_None, true, false, false, false, 0.f, 0.f };
			Input move_left{ ImGuiKey_A, ImGuiKey_None, ImGuiKey_GamepadLStickLeft, ImGuiKey_None, true, false, false, false, 0.f, 0.f };
			Input move_right{ ImGuiKey_D, ImGuiKey_None, ImGuiKey_GamepadLStickRight, ImGuiKey_None, true, false, false, false, 0.f, 0.f };

			void actions()
			{
				if (open)
				{
					sounds::queue(g_open ? sounds::g_open : sounds::g_close);
					g_open ^= true;
				}
				if (g_open)
				{
					if (enter)
					{
						sounds::queue(sounds::g_enter);
						g_current_menu->action(ActionType::Enter);
					}
					if (back)
					{
						sounds::queue(sounds::g_back);
						menu::pop();
					}
					if (up)
					{
						sounds::queue(sounds::g_up);
						g_current_menu->action(ActionType::Up);
					}
					if (down)
					{
						sounds::queue(sounds::g_down);
						g_current_menu->action(ActionType::Down);
					}
					if (left)
					{
						sounds::queue(sounds::g_left);
						g_current_menu->action(ActionType::Left);
					}
					if (right)
					{
						sounds::queue(sounds::g_right);
						g_current_menu->action(ActionType::Right);
					}
				}
			}
		}
	}
}
