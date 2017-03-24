#include <jogurt_math.h>
#include "opengl.h"

using namespace jogurt;

int main(int argc, char** argv)
{
    GLFWwindow* window;
    vec2f x = zero2f();

    if (!glfwInit())
        return 1;

    window = glfwCreateWindow(800, 600, "Szynka", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
