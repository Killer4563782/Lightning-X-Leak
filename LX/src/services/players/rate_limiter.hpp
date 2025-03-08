#pragma once
namespace services
{
    class rate_limiter
    {
        uint32_t m_attempts_allowed_in_time_period;
        milliseconds m_time_period;
        system_clock::time_point m_last_event_time{};
        uint32_t m_num_attempts_allowed = 0;

    public:
        rate_limiter(milliseconds time_period, uint32_t num_allowed_attempts) :
            m_attempts_allowed_in_time_period(num_allowed_attempts),
            m_time_period(time_period)
        {}

        // Returns true if the rate limit has been exceeded
        // Sets already_limited to true if the rate limit was already exceeded before the current call. Use this arg to prevent notification spam
        bool process(bool* already_limited = nullptr)
        {
            if (system_clock::now() - m_last_event_time < m_time_period)
            {
                if (++m_num_attempts_allowed > m_attempts_allowed_in_time_period)
                {
                    if (already_limited)
                    {
                        if (m_num_attempts_allowed - 1 > m_attempts_allowed_in_time_period)
                            *already_limited = true;

                        else *already_limited = false;
                    }

                    m_last_event_time = system_clock::now();
                    return true;
                }
            }
            else
            {
                m_last_event_time = system_clock::now();
                m_num_attempts_allowed = 1;
            }
            return false;
        }

        // Check if the rate limit was exceeded by the last process() call. Use this to prevent the player from being flooded with notifications
        [[nodiscard]] bool exceeded_last_process() const
        {
            return m_num_attempts_allowed - 1 == m_attempts_allowed_in_time_period;
        }
    };
}