#pragma once
#include <format>
#include <source_location>
#include <iostream>

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

    /**
     * @brief Simple static logger object
     * 
     */
    struct logger
    {
    private:
        /**
         * @brief Get current time
         * 
         * @param tp Chrono time point
         * @return Returning zoned time point 
         */
        static auto get_time(std::chrono::system_clock::time_point const tp) -> decltype(auto)
        {
            return std::chrono::zoned_time{std::chrono::current_zone(), tp};
        }

        /**
         * @brief Converting time point to string
         * 
         * @param tp Chrono time point
         * @return std::string Formatted string with time point data
         */
        static std::string to_string(auto tp)
        {
            return std::format("{:%F %T %Z}", tp);
        }

        /**
         * @brief Converting current file location data to string
         * 
         * @param source Source_location data
         * @return std::string Formatted string with cata about current file location
         */
        static std::string to_string(std::source_location const source)
        {
            std::string sfile = source.file_name();
            return std::format("{}:{}:{}", sfile.substr(sfile.find_last_of("/\\") + 1), source.function_name(), source.line());
        }
    public:
        /**
         * @brief Push log in console output
         * 
         * @param level Log level (see ELogLevel enum class)
         * @param message Output message
         * @param source Current source location
         */
        static void log(ELogLevel const level, std::string_view const message, std::source_location const source = std::source_location::current())
        {
            std::cout << std::format("[{}] {} | {} | {}", static_cast<char>(level), to_string(get_time(std::chrono::system_clock::now())), to_string(source), message) << '\n';
            std::cout.flush();
        }
    };
}