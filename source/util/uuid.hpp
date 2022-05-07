#pragma once
#include <random>
#include <sstream>

/**
 * @brief UUID generation helper
 * 
 */
struct uuid 
{
    static std::random_device              rd;
    static std::mt19937_64                    gen;
    static std::uniform_int_distribution<> dis;
    static std::uniform_int_distribution<> dis2;

    /**
     * @brief Generating UUID
     * 
     * @return std::string UUIDv4
     */
    static std::string generate();
};