cmake_minimum_required(VERSION 3.11)
project(vulkan VERSION 1.0.0 DESCRIPTION "BouwnLaw's attempt at Vulkan" LANGUAGES CXX)

add_library(vulkan STATIC IMPORTED GLOBAL) # The Vulkan library is already built outside of the CMake project. We'll import it as a CMake target for convenience.

if(WIN32)
	target_include_directories(vulkan INTERFACE win32/Include) # Anyone that uses this library will have these headers.
	if (CMAKE_SIZEOF_VOID_P EQUAL 8) # If x64.
		set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/win32/Lib/vulkan-1.lib") # Set the location of the imported library.
	else ()
		set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/win32/Lib32/vulkan-1.lib") # Set the location of the imported library.
	endif ()
elseif(APPLE)
	target_include_directories(vulkan INTERFACE macos/macOS/include) # Anyone that uses this library will have these headers.
	set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/macos/macOS/lib/libvulkan.1.1.77") # Set the location of the imported library.
else()
	target_include_directories(vulkan INTERFACE linux/x86_64/include) # Anyone that uses this library will have these headers.
	if (CMAKE_SIZEOF_VOID_P EQUAL 8) # If x64.
		set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR/linux/x86_64/lib/libvulkan.so.1.1.77") # Set the location of the imported library.
	else ()
		set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR/linux/source/lib/libvulkan.so.1.1.77") # Set the location of the imported library.
	endif ()
endif()
