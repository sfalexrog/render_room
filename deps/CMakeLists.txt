# Dependency book-keeping for the raymarching project

# Ignore glm's CMakeLists.txt file, it tends to add targets we don't need
add_library(Glm INTERFACE)
target_include_directories(Glm INTERFACE glm)

# imgui does not have a library :-(

add_library(Imgui STATIC "")
target_sources(Imgui PRIVATE
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        )
target_include_directories(Imgui PUBLIC
        imgui
        )

add_subdirectory(glbinding)
