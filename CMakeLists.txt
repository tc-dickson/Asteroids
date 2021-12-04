add_library(asteroidsGame linearAlg.c spaceship.c)
target_link_libraries(asteroidsGame ${330_LIBS})

add_executable(asteroids.elf main.c)
target_link_libraries(asteroids.elf ${330_LIBS} asteroidsGame buttons_switches)
set_target_properties(asteroids.elf PROPERTIES LINKER_LANGUAGE CXX)