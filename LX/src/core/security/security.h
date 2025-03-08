#pragma once

namespace security
{
	inline bool tripped_dbg{};
	enum class DetectReason
	{
		DebuggerPresent,
		OutputDbgString,
		DbgBreak,
		DbgBreakPoint,
		HashInvalid
	};

	void bsod();
	void debugger_detected(DetectReason code);
	bool check_for_debugger();

	void defeat_guardit();
	void attempt_to_yeet_rage_sec();

	void init();
	void uninit();
}