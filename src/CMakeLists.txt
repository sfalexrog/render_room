project(cmc_raymarch_impl)

add_executable(cmc_raymarch "")

target_sources(cmc_raymarch PRIVATE
		main.cpp
		ui/imgui_impl.cpp
		ui/LogWindow.cpp
		ui/LogWindow.h
		ui/MapWindow.cpp
		ui/MapWindow.h
		render/camera.cpp
		render/camera.h
		render/compute.cpp
		render/render.cpp
		render/shader.cpp
		render/shader.h
		render/voxel_map.cpp
		render/voxel_map.h
		util/Logger.cpp
		util/Logger.h
		format/mesh.h
		format/wavefront_obj_loader.cpp
		format/wavefront_obj_loader.h
		)

target_include_directories(cmc_raymarch
		PRIVATE
		${PROJECT_SOURCE_DIR}
		)

target_link_libraries(cmc_raymarch
		Sdl2Static
		Imgui
		Glm
		glbinding
		)

