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

# RapidJSON seems to be a header-only library, much like glm
add_library(RapidJson INTERFACE)
target_include_directories(RapidJson INTERFACE rapidjson/include)

add_subdirectory(glbinding)
