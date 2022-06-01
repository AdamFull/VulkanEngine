#pragma once

namespace engine
{
    namespace math
    {
        template<class _Ty>
        _Ty rangeToRange(_Ty input, _Ty in_min , _Ty in_max, _Ty out_min, _Ty out_max)
        {
            return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }
    }
}