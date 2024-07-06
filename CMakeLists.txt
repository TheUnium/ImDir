cmake_minimum_required(VERSION 3.10)
project(ImFileManager)

set(CMAKE_CXX_STANDARD 17)

# Add ImGui source files
file(GLOB IMGUI_SRC
        "imgui/imgui.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_widgets.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/backends/imgui_impl_glfw.cpp"
        "imgui/backends/imgui_impl_opengl3.cpp")

include_directories(imgui imgui/backends icons include)

# Add the executable
add_executable(ImFileManager
        src/main.cpp
        src/ui.cpp
        src/file_operations.cpp
        src/terminal.cpp
        ${IMGUI_SRC})

# Link necessary libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(ImFileManager glfw OpenGL::GL pthread)
