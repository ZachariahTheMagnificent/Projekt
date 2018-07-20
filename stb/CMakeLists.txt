cmake_minimum_required( VERSION 3.9 )
project( stb )

add_library( stb INTERFACE )

target_include_directories( stb INTERFACE include )
target_compile_features( stb INTERFACE cxx_std_17 )