#include "core/Application.h"
#include "Resources/ResourceManager.h"

int main()
{
    //Engine::ResourceManager::GetInstance()->Load("../../assets/resources.json");
    Engine::Application app;
    app.Create();

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