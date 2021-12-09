add_library(asteroidFunction asteroid.c game.c)
add_executable(lab7.elf main.c)
target_link_libraries(lab7.elf ${330_LIBS} intervalTimer clockFunction asteroidFunction buttons_switches)
set_target_properties(lab7.elf PROPERTIES LINKER_LANGUAGE CXX)