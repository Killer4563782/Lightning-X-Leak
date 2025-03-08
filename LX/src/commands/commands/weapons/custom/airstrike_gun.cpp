#include "commands/types/looped_command.hpp"
#include "util/util.h"

constexpr auto hash = "vehicle_weapon_space_rocket"_joaat;

namespace commands
{
    class airstrike_gun final : public looped_command
    {
    public:
        using looped_command::looped_command;

    private:
        void on_tick() override
        {
            // Request the weapon asset
            if (!util::natives::request_weapon_asset(hash))
                return;

            // Get the last weapon impact coordinates
            Vector3 bullet_coord{};
            if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(self::ped, &bullet_coord))
            {
                const auto damage = static_cast<int>(WEAPON::GET_WEAPON_DAMAGE(hash, 0));

                // Define offsets and speeds for the airstrike bullets
                constexpr std::array bullet_params = {
                    std::make_tuple(0.0f, 0.0f, 15.0f, 100.0f),
                    std::make_tuple(0.0f, 5.0f, 15.0f, 130.0f),
                    std::make_tuple(3.0f, 0.0f, 15.0f, 80.0f),
                    std::make_tuple(-5.0f, 0.0f, 15.0f, 110.0f),
                    std::make_tuple(0.0f, -2.0f, 15.0f, 120.0f)
                };

                // Fire bullets with the defined parameters
                for (const auto& [x_offset, y_offset, z_offset, speed] : bullet_params)
                {
                    MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(
                        bullet_coord.x + x_offset, bullet_coord.y + y_offset, bullet_coord.z + z_offset,
                        bullet_coord.x, bullet_coord.y, bullet_coord.z,
                        damage, 1, hash, self::ped, FALSE, TRUE, speed
                    );
                }
            }
        }

        void on_disable() override
        {
            // Remove the weapon asset when the command is disabled
            WEAPON::REMOVE_WEAPON_ASSET(hash);
        }
    };

    airstrike_gun g_airstrike_gun("Airstrike Gun", "airstrike_gun", {});
}