cmake_minimum_required( VERSION 3.10 )
project( Projekt )

set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Do not build documents")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Do not build tests")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Do not build example programs")
set(GLFW_INSTALL OFF CACHE INTERNAL "Do not generate installation target")

add_subdirectory( glfw ) # Build the glfw project.
add_subdirectory( vulkan ) # Build the vulkan project.
add_subdirectory( glm ) # Build the glm project.
add_subdirectory( stb ) # Build the stb project.
add_subdirectory( engine ) # Build the engine project.
add_subdirectory( game ) # Build the game project.