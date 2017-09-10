#include <iostream>
#include <jogurt_math.h>
#include "opengl.h"
#include "assets.h"

using namespace szynka;
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

    Assets::add_config("szynka_assets.config");
    std::cout << Assets::read_all_text("hello.txt", "generic") << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
