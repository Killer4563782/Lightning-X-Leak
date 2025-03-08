#include "commands/types/looped_vector_command.hpp"
#include "helpers/teleportHelpers.h"
#include "util/util.h"

namespace commands
{
	std::vector<std::string> auto_teleport_options = { "Waypoint", "Objective", "Both" };

    class auto_teleport final : looped_vector_command
    {
        using looped_vector_command::looped_vector_command;

        void on_tick() override
        {
            if (HUD::IS_PAUSE_MENU_ACTIVE())
                return;

            util::blip way_point{ BlipSpriteWaypoint, util::eBlipHandleType::first };
            util::blip objective{ BlipColorMission, util::eBlipHandleType::first };

            switch (m_index)
            {
	            case 0: //Waypoint
	            {
                    if (way_point.exists())
                    {
	                    teleport::teleport_to_waypoint();
                    }
	                break;
	            }
				case 1: //Objective
				{
					if (const auto& obj_pos = objective.coords(); objective.exists() && SYSTEM::VDIST(obj_pos.x, obj_pos.y, obj_pos.z, self::position.x, self::position.y, self::position.z) > 20.f)
                    {
						teleport::teleport_to_objective();
                    }
					break;
				}
				case 2: //Both
		        {
                    if (way_point.exists())
                    {
                        return teleport::teleport_to_waypoint();
                    }
					if (const auto& obj_pos = objective.coords(); objective.exists() && SYSTEM::VDIST(obj_pos.x, obj_pos.y, obj_pos.z, self::position.x, self::position.y, self::position.z) > 20.f)
                    {
						return teleport::teleport_to_objective();
					}
					break;
		        }
            default: ;
            }
        }
    };

    auto_teleport g_auto_teleport("Auto Teleport", "auto_teleport", "Automatically teleport to waypoint or objective", false, false, auto_teleport_options);
}