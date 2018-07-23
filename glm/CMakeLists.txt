cmake_minimum_required(VERSION 3.11)
project(glm VERSION 1.0.0 DESCRIPTION "BouwnLaw's attempt at Vulkan" LANGUAGES CXX)

add_library(glm INTERFACE) # This is a header only library so it only needs to be an INTERFACE library that doesn't build anything.
target_include_directories(glm INTERFACE include) # Give anyone that uses this library our headers.
target_compile_features(glm INTERFACE cxx_std_17) # Anyone that uses this libary will use C++17.
