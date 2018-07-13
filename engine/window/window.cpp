/*!
 *
 */

#include <cassert>

#include "window.h"
#include "glfw_callbacks.h"

#include "../utils/exception/glfw_exception.h"
#include "../utils/exception/vulkan_exception.h"

std::vector<event> event_handler::event_queue_;

window::window( std::uint32_t width, std::uint32_t height, const std::string &title )
    :
    width_( width ),
    height_( height ),
    title_( title )
{
    assert( width_ > 0 && "Width too small!" );
    assert( height_ > 0 && "Height too small!" );

    set_up();
}

window::window( std::uint32_t x_pos, std::uint32_t y_pos,
                std::uint32_t width, std::uint32_t height,
                const std::string &title )
    :
    x_pos_( x_pos ),
    y_pos_( y_pos ),
    width_( width ),
    height_( height ),
    title_( title )
{
    assert( width_ > 0 && "Width too small!" );
    assert( height_ > 0 && "Height too small!" );

    set_up();
}

window::~window( )
{
    glfwDestroyWindow( p_window_ );
    glfwTerminate( );
}

void
window::poll_event( )
{
    glfwPollEvents();
}
void
window::handle_event( event& e )
{
    if ( e.event_type == event::type::window_resized )
    {
        width_ = e.window_resize.width;
        height_ = e.window_resize.height;
    }
    else if( e.event_type == event::type::window_moved )
    {
        x_pos_ = e.window_move.x;
        x_pos_ = e.window_move.y;
    }
}

bool
window::is_open( ) const
{
    return !glfwWindowShouldClose( p_window_ );
}

std::vector<const char*>
window::get_required_extensions( ) const
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions( &glfw_extension_count );

    std::vector<const char*> extensions( glfw_extensions, glfw_extensions + glfw_extension_count );

    if( enable_validation_layers )
        extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

    return extensions;
}

const std::uint32_t
window::get_width( ) const
{
    return width_;
}

const std::uint32_t
window::get_height( ) const
{
    return height_;
}

const std::string&
window::get_title( ) const
{
    return title_;
}


void
window::set_up( )
{
    if( !glfwInit( ) )
        throw glfw_exception{ "Failed to initialize GLFW!", __FILE__, __LINE__ };

    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

    p_window_ = glfwCreateWindow( width_, height_, title_.c_str(), nullptr, nullptr );
    if( p_window_ == nullptr )
    {
        glfwDestroyWindow( p_window_ );
        glfwTerminate( );
    }

    glfwSetWindowPos( p_window_, x_pos_, y_pos_ );

    set_window_position_callback( glfw_callbacks::window_pos_callback );
    set_window_resize_callback( glfw_callbacks::window_size_callback );
    set_frame_buffer_resize_callback( glfw_callbacks::frame_buffer_size_callback );
}

VkSurfaceKHR
window::create_surface( const VkInstance &instance_handle ) const
{
    VkSurfaceKHR surface;

	if( glfwCreateWindowSurface( instance_handle, p_window_, nullptr, &surface ) != VK_SUCCESS )
        throw vulkan_exception{ "Failed to create surface", __FILE__, __LINE__ };

    return surface;
}

void
window::set_window_resize_callback( GLFWwindowsizefun window_resize_callback )
{
    glfwSetWindowSizeCallback( p_window_, window_resize_callback );
}

void
window::set_window_position_callback( GLFWwindowposfun window_position_callback )
{
    glfwSetWindowPosCallback( p_window_, window_position_callback );
}

void
window::set_frame_buffer_resize_callback( GLFWframebuffersizefun framebuffer_size_callback )
{
    glfwSetFramebufferSizeCallback( p_window_, framebuffer_size_callback );
}
