cmake_minimum_required( VERSION 3.9 )
project( glm )

add_library( glm INTERFACE )

target_include_directories( glm INTERFACE include )
target_compile_features( glm INTERFACE cxx_std_17 )