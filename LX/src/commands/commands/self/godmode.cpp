#include "commands/types/looped_command.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

namespace commands
{
    class godmode final : public looped_command
    {
    public:
        using looped_command::looped_command;

    private:
        uint32_t last_bits = 0;

        void on_enable() override
        {
            g_script_patcher_service->update();
        }

        void on_tick() override
        {
            if (!self::local_ped)
                return;

            const uint32_t bits = God;
            const uint32_t changed_bits = bits ^ last_bits;
            const uint32_t changed_or_enabled_bits = bits | changed_bits;

            if (changed_or_enabled_bits)
            {
                const uint32_t unchanged_bits = self::local_ped->m_damage_bits & ~changed_or_enabled_bits;
                self::local_ped->m_damage_bits = unchanged_bits | bits;
                last_bits = bits;
            }
        }

        void on_disable() override
        {
            g_script_patcher_service->update();

            if (self::local_ped)
                self::local_ped->m_damage_bits = 0;
        }
    };

    godmode g_godmode("Godmode", "godmode", {});
}