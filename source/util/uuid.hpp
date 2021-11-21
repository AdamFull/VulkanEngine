#pragma once
#include <random>
#include <sstream>

struct uuid 
{
    static std::random_device              rd;
    static std::mt19937                    gen;
    static std::uniform_int_distribution<> dis;
    static std::uniform_int_distribution<> dis2;

    static std::string generate();
};