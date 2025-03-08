#pragma once

class simpleTimer
{
	high_resolution_clock::time_point m_timer;
	high_resolution_clock::duration m_delay;

public:
	explicit simpleTimer(milliseconds delay);
	bool update();
	void set_delay(int delay);
	void set_delay(milliseconds delay);
};