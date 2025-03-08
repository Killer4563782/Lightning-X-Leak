#include "commands/types/looped_vector_command.hpp"
#include "memory/pointers.h"
#include "services/script_patcher/script_patcher_service.hpp"

namespace commands
{
	class invisible final : public looped_vector_command {
	public:
		using looped_vector_command::looped_vector_command;

	private:
		enum class visibility_state : int {
			locally_visible = 0,
			locally_transparent = 1,
			fully_invisible = 2
		};

		void on_enable() override
		{
			g_script_patcher_service->update();
		}

		void on_tick() override
		{
			ENTITY::SET_ENTITY_VISIBLE(self::ped, !*g_pointers->g_is_session_active, 0);

			switch (static_cast<visibility_state>(m_index))
			{
				case visibility_state::locally_visible:
				{
					NETWORK::SET_ENTITY_LOCALLY_VISIBLE(self::ped);
					ENTITY::RESET_ENTITY_ALPHA(self::ped);
				}
				break;
				case visibility_state::locally_transparent:
				{
					NETWORK::SET_ENTITY_LOCALLY_VISIBLE(self::ped);
					ENTITY::SET_ENTITY_ALPHA(self::ped, 150, FALSE);
				}
				break;
				case visibility_state::fully_invisible:
				{
					ENTITY::RESET_ENTITY_ALPHA(self::ped);
					if (!*g_pointers->g_is_session_active)
					{
						ENTITY::SET_ENTITY_ALPHA(self::ped, 0, FALSE);
					}
				}
				break;
			}
		}

		void on_disable() override
		{
			g_script_patcher_service->update();

			ENTITY::RESET_ENTITY_ALPHA(self::ped);
			ENTITY::SET_ENTITY_VISIBLE(self::ped, true, 0);
		}
	};

	invisible g_invisible("invisible", "invisible", "Become invisible", false, false, { "Locally Visible", "Locally Transparent", "Fully Invisible" }, 0);
}