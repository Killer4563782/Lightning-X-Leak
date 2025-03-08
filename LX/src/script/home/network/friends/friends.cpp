#include "friends.h"

#include "gui/interface.h"
#include "gui/types/function_option.hpp"
#include "gui/types/vector_option.hpp"
#include "helpers/playerInfo.h"
#include "services/api/api_service.hpp"
#include "services/online/online.h"
#include "thread/threadpool.h"

namespace ui::submenus
{
	void join_option::action(ActionType type, bool first_receiver)
	{
		switch (type)
		{
		case ActionType::Left:
		{
			*m_index = *m_index > 0 ? *m_index - 1 : static_cast<int>(m_vector->size()) - 1;
			break;
		}
		case ActionType::Right:
		{
			*m_index = *m_index >= static_cast<int>(m_vector->size()) - 1 ? 0 : *m_index + 1;
			break;
		}
		case ActionType::Enter:
		{
			if (m_function)
				m_function();
		}
		default: ;
		}
	}

	friend_submenu::friend_submenu(FriendInfo* friend_info, std::string description, submenu* parent) : submenu(friend_info->m_name, std::move(description), parent),
		m_friend(friend_info)
	{}

	void friend_submenu::on_enter()
	{
		m_options.clear();

		if (m_friend->m_is_joinable)
		{
			this->add(join_option("Join", std::format("Join {}'s session", m_friend->m_name), &g_api_service->rid_joiner_idx, &g_api_service->rid_joiner_type, [&]
			{
				g_thread_pool->push([&]
				{
					g_api_service->rid_join(m_friend->m_rockstar_id);
				});
			}));
		}
		if (services::is_online())
		{
			this->add(function_option("Invite", {}, [this]
			{
				g_thread_pool->push([&]
				{
					g_api_service->rid_invite(m_friend->m_rockstar_id);
				});
			}));
		}
		this->add(function_option("Copy Name", {}, [this]
		{
			ImGui::SetClipboardText(m_friend->m_name);
			notify(Success, 5s, "Copied {}'s name", m_friend->m_name);
		}));
		this->add(function_option("Copy RID", {}, [this]
		{
			ImGui::SetClipboardText(std::to_string(m_friend->m_rockstar_id).c_str());
			notify(Success, 5s, "Copied {}'s RID | {}", m_friend->m_name, m_friend->m_rockstar_id);
		}));
		this->add(function_option("Open Profile", {}, [this]
		{
			services::open_profile_by_rid(m_friend->m_rockstar_id);
		}));
	}

	void friend_submenu::on_tick()
	{
		player_info::friend_info(m_friend);
	}

	void friends_submenu::on_enter()
	{
		m_options.clear();

		if (auto friend_count = services::get_friends_count(); friend_count != NULL)
		{
			for (uint32_t i = 0; i < friend_count; i++)
			{
				const auto friend_info = services::get_friend(i);
				this->add(friend_submenu(friend_info, {}));
			}
		}
		else
		{
			this->add(option("No Friends Found", "Add friends to display them here"));
		}
	}

	friends_submenu g_friends_submenu("Friends", {});
}
