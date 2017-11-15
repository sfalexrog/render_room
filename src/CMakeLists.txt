add_executable(cmc_raymarch "")

target_sources(cmc_raymarch PRIVATE
	main.cpp)


target_link_libraries(cmc_raymarch Sdl2Static)

