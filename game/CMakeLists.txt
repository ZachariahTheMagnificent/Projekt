cmake_minimum_required( VERSION 3.10 )
project( game )

add_executable( game
        game/main.cpp
        game/game.h
        game/game.cpp )

#target_include_directories( engine PUBLIC graphics PUBLIC utils PUBLIC vulkan PUBLIC window )
target_compile_features( game PUBLIC cxx_std_17 )
target_link_libraries( game engine vulkan glfw glm stb )