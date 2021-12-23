#pragma once

namespace Engine::Math
{
    template<class Type>
    Type RangeToRange(Type input, Type in_min , Type in_max, Type out_min, Type out_max)
    {
        return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}