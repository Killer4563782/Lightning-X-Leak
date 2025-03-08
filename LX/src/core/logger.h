#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/wincolor_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#pragma warning(disable:4996)

#define LOG(l, s, ...) spdlog::##l##(s, __VA_ARGS__)
#ifdef _DEBUG
#define LOG_DEBUG(s, ...) spdlog::debug(s, __VA_ARGS__)
#else
#define LOG_DEBUG(s, ...)
#endif

class logger
{
public:
	logger(const std::string& title, const std::filesystem::path& log_path)
	{
		auto log_file_path = log_path / "Log.txt";
		auto log_prev_path = log_path / "Log-Prev.txt";

		if (exists(log_file_path) && !is_empty(log_file_path))
		{
			std::ofstream log_prev(log_prev_path, std::ios::out | std::ios::app);
			std::ifstream log(log_file_path, std::ios::in);

			if (!is_empty(log_prev_path))
				log_prev << "\n\n";

			log_prev << log.rdbuf();
		}

		m_file.open(log_file_path, std::ios::out | std::ios::trunc);

		setup_console(title);
		setup_logger();
	}

	~logger()
	{
		FreeConsole();
		fclose(stdout);
		fclose(stdin);
		m_console.close();
		m_file.close();
		spdlog::shutdown();
	}

private:
	FILE* m_stdout_stream{};
	FILE* m_stdin_stream{};
	std::ofstream m_console;
	std::ofstream m_file;

	void setup_console(const std::string& title)
	{
		if (!AttachConsole(ATTACH_PARENT_PROCESS))
		{
			AllocConsole();
			SetConsoleTitleA(title.c_str());
			SetConsoleCP(CP_UTF8);
			SetConsoleOutputCP(CP_UTF8);
		}
		m_console.open("CONOUT$");
		freopen_s(&m_stdout_stream, "CONOUT$", "w", stdout);
		freopen_s(&m_stdin_stream, "CONIN$", "r", stdin);
	}

	void setup_logger()
	{
		auto con_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
		auto file_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(m_file, true);

		spdlog::sinks_init_list sinks = {con_sink, file_sink};
		auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
#ifdef _DEBUG
        logger->set_level(spdlog::level::trace);
#else
		logger->set_level(spdlog::level::info);
#endif
		set_default_logger(logger);
	}
};

inline std::unique_ptr<logger> g_logger;