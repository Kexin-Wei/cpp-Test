#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>  // or "../stdout_sinks.h" if no colors needed
#include <spdlog/spdlog.h>

#define log_dev_trace(...) \
    SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__)
#define log_dev_debug(...) \
    SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#define log_dev_info(...) \
    SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#define log_dev_warning(...) \
    SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#define log_dev_error(...) \
    SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#define log_dev_critical(...) \
    SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)

#include <chrono>
#include <ctime>
#include <filesystem>
#include <string>

namespace spdlog {
inline void init_logger() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm *localTime = std::localtime(&currentTime);
    std::string currentDateTime = std::asctime(localTime);
    currentDateTime.pop_back();  // remove newline character
    // std::string fileName = "logs/log_" + currentDateTime + ".log"; //Windows has error but no in linux
    std::string fileName = "logs/log.log";
    // std::string devFileName = "logs/log_dev_" + currentDateTime + ".log";
    std::string devFileName = "logs/log_dev.log";

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("[%T.%e][%^%l%$] %v");

    auto fileSink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
    fileSink->set_pattern("[%Y-%m-%d %T.%e][%^%l%$] %v");

    auto developerSink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(devFileName, true);
    developerSink->set_pattern(
        "[%Y-%m-%d %T.%e][%^%l%$][thread %t][file:%s - line:%#] %v");

    // set multi_sink logger as default logger
    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({
                                                           consoleSink,
                                                           fileSink,
                                                           developerSink,
                                                       })));
    spdlog::set_level(spdlog::level::trace);
    consoleSink->set_level(spdlog::level::debug);
    fileSink->set_level(spdlog::level::trace);
    developerSink->set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
};
}  // namespace spdlog
namespace Logger = spdlog;