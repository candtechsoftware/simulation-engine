cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
project(simulationengine)
add_definitions("-Wall" "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # creates compile_command.json

# GLFW and GLM 
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

FILE(GLOB SOURCE_FILES src/*.cpp)


# vulkan
find_package(Vulkan REQUIRED)


include_directories(include)

add_executable(simulationengine ${SOURCE_FILES})

target_include_directories(simulationengine
  PUBLIC ${VULKAN_INCLUDE_DIRS} 
  PUBLIC ${GLFW_INCLUDE_DIRS} 
  PUBLIC ${GLM_INCLUDE_DIRS} 
)

target_link_libraries(simulationengine glfw)
target_link_libraries(simulationengine  Vulkan::Vulkan)

