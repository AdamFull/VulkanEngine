#pragma once
#include <chrono>
#include <format>
#include <source_location>

namespace utl
{
    enum class ELogLevel : char
    {
        eVerbose = 'V',
        eInfo = 'I',
        eDebug = 'D',
        eWarning = 'W',
        eError = 'E'
    };

    struct logger
    {
    private:
        static auto get_time(std::chrono::system_clock::time_point const tp) -> decltype(auto)
        {
            return std::chrono::zoned_time{std::chrono::current_zone(), tp};
        }

        static std::string to_string(auto tp)
        {
            return std::format("{:%F %T %Z}", tp);
        }

        static std::string to_string(std::source_location const source)
        {
            return std::format("{}:{}:{}", source.file_name(), source.function_name(), source.line());
        }
    public:
        static void log(ELogLevel const level, std::string_view const message, std::source_location const source = std::source_location::current())
        {
            std::cout << std::format("[{}] {} | {} | {}", static_cast<char>(level), to_string(get_time(std::chrono::system_clock::now())), to_string(source), message) << '\n';
        }
    };
}