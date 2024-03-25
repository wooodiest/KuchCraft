#include "kcpch.h"

#include "Core/Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    KuchCraft::Log::Init();

    KC_PROFILE_BEGIN_SESSION("Startup", "KuchCraft-Profile-Startup.json");
    auto application = new KuchCraft::Application();
    KC_PROFILE_END_SESSION();

    KC_PROFILE_BEGIN_SESSION("Runtime", "KuchCraft-Profile-Runtime.json");
    application->Run();
    KC_PROFILE_END_SESSION();

    KC_PROFILE_BEGIN_SESSION("Shutdown", "KuchCraft-Profile-Shutdown.json");
    delete application;
    KC_PROFILE_END_SESSION();

    return 0;
}