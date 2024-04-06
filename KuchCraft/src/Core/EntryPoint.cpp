#include "kcpch.h"

#include "Core/Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    KuchCraft::Log::Init();

    auto application = new KuchCraft::Application();
    application->Run();
    delete application;

    return 0;
}