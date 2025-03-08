#pragma once
#include "gui/types/submenu.hpp"
#include "gui/types/vector_option.hpp"

namespace ui::submenus
{
	class join_option final : public vector_option
	{
		using vector_option::vector_option;

		void action(ActionType type, bool first_receiver) override;
	};


	class friend_submenu final : public submenu
	{
	public:
		FriendInfo* m_friend;

		explicit friend_submenu(FriendInfo* friend_info, std::string description, submenu* parent = nullptr);

		void on_enter() override;
		void on_tick() override;
	};


	class friends_submenu final : public submenu
	{
		using submenu::submenu;
		
		void on_enter() override;
	};

	extern friends_submenu g_friends_submenu;
}
