#include "memory.hpp"
#include "ulog.hpp"

namespace utl
{
    track_printer::~track_printer()
    {
        track_type::const_iterator it = track->begin();
        while (it != track->end())
        {
            std::stringstream ss;
            ss << "TRACK: leaked at " << ", " << " bytes\n"; //((unsigned)it->first) << it->second
            utl::logger::log(ELogLevel::eWarning, ss.str());
            ++it;
        }
    }
}

utl::track_type *get_map()
{
    // don't use normal new to avoid infinite recursion.
    static utl::track_type *track = new (std::malloc(sizeof(*track))) utl::track_type;
    static utl::track_printer printer(track);
    return track;
}

void *operator new(std::size_t size) throw(std::bad_alloc)
{
    // we are required to return non-null
    void *mem = std::malloc(size == 0 ? 1 : size);
    if (mem == 0)
    {
        throw std::bad_alloc();
    }
#ifdef _DEBUG
    //(*get_map())[mem] = size;
#endif
    return mem;
}

void operator delete(void *mem) throw()
{
#ifdef _DEBUG
    /*if (get_map()->erase(mem) == 0)
    {
        char buffer[256];
        std::sprintf(buffer, "bug: memory at  %p wasn't allocated by us", mem);
        utl::logger::log(utl::ELogLevel::eWarning, buffer);
    }*/
#endif
    std::free(mem);
}