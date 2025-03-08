#include "simpleTimer.h"

simpleTimer::simpleTimer(milliseconds delay) :
	m_timer(high_resolution_clock::now()),
	m_delay(std::chrono::duration_cast<high_resolution_clock::duration>(delay))
{}

bool simpleTimer::update()
{
	if (const auto now = high_resolution_clock::now(); (now.time_since_epoch() - m_timer.time_since_epoch()).count() >= m_delay.count())
	{
		m_timer = now;
		return true;
	}

	return false;
}

void simpleTimer::set_delay(int delay)
{
	m_delay = milliseconds(delay);
}

void simpleTimer::set_delay(milliseconds delay)
{
	m_delay = delay;
}
