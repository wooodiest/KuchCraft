#include "Core/Window.h"
#include <glad/glad.h>

int main(void)
{
    KuchCraft::Window window("KuchCraft", 1600, 900, true);

    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        glClearColor(0.8f, 0.3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.OnUpdate();
    }

    glfwTerminate();
    return 0;
}