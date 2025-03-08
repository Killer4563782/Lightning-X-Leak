#include <algorithm>
#include "commands/types/command.hpp"

namespace commands
{
    class set_wanted_level final : public command {
    public:
        using command::command;
    private:
        void execute(const command_arguments& args) override
        {
            const uint32_t wanted_level = std::min<uint32_t>(args.get<uint32_t>(0), 5);

            if (self::local_ped && self::local_ped->m_player_info)
            {
                self::local_ped->m_player_info->m_wanted_level = wanted_level;
                self::local_ped->m_player_info->m_is_wanted = wanted_level > 0;
            }
        }
    };

    set_wanted_level g_set_wanted_level("Set Wanted Level", "set_wanted_level", "Set your wanted level", 1);
}