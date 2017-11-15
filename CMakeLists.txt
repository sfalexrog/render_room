cmake_minimum_required(VERSION 3.6)
project(cmc_raymarch)

if (NOT EXISTS deps/SDL/CMakeLists.txt)
	if (NOT EXISTS tmp/SDL-2.0.7.tar.gz)
		message(STATUS "Requesting SDL sources from official site")
		file(DOWNLOAD http://libsdl.org/release/SDL2-2.0.7.tar.gz tmp/SDL-2.0.7.tar.gz SHOW_PROGRESS)
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/deps/SDL)
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf tmp/SDL-2.0.7.tar.gz)
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory SDL2-2.0.7 ${PROJECT_SOURCE_DIR}/deps/SDL)
endif()

add_subdirectory(deps/SDL)
# Patch SDL's targets
target_include_directories(SDL2main PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/SDL/include>)
target_include_directories(SDL2 PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/SDL/include>)
target_include_directories(SDL2-static PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/SDL/include>)
add_library(Sdl2 INTERFACE)
add_library(Sdl2Static INTERFACE)
target_link_libraries(Sdl2 INTERFACE SDL2 SDL2main)
target_link_libraries(Sdl2Static INTERFACE SDL2-static SDL2main)

add_subdirectory(src)
