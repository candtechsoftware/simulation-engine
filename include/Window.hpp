#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Simulation {
class Window {
  public:
  Window(int width, int height, const std::string &name);
  ~Window();
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }

  private:
  void initWindow();

  GLFWwindow *window;
  const int m_width;
  const int m_height;
  std::string m_name;
};
}  // namespace Simulation

