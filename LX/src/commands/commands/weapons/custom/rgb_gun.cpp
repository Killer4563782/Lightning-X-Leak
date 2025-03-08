#include "commands/types/looped_command.hpp"
#include <cstdlib> // Added for std::rand/srand
#include <ctime>   // Added for time()

namespace commands
{
    class rgb_gun final : public looped_command
    {
        using looped_command::looped_command;

        void on_tick() override
        {
            Hash weapon_hash;
            if (WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, FALSE))
            {
                std::srand(static_cast<unsigned>(time(nullptr))); // Seed added
                WEAPON::SET_PED_WEAPON_TINT_INDEX(
                    self::ped, 
                    weapon_hash, 
                    std::rand() % 8 // Changed to standard random
                );
            }
        }
    };

    rgb_gun g_rgb_gun("RGB Gun", "rgb_gun", {});
}