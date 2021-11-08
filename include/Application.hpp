#pragma once

#include "Window.hpp"

namespace Simulation {
class Application {
  public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 800;

  void run();

  private:
  Window m_window{WIDTH, HEIGHT, "Hello Vulkan"};
};
}  // namespace Simulation
