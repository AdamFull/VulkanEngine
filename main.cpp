#include "core/Application.h"
#include "Resources/ResourceManager.h"
#include "util/ulog.hpp"

int main()
{
    Engine::CApplication app;
    app.create();

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        utl::logger::log(utl::ELogLevel::eError, e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}