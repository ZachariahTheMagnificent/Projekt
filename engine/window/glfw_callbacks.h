/*!
 *
 */

#ifndef PROJEKT_GLFW_CALLBACKS_H
#define PROJEKT_GLFW_CALLBACKS_H

#include <glfw/glfw3.h>

struct glfw_callbacks
{
    static void
    window_pos_callback( GLFWwindow* p_window, int x, int y )
    {
        event e;
        e.window_move.x = static_cast<uint32_t>( x );
        e.window_move.y = static_cast<uint32_t>( y );
        e.event_type = event::type::window_moved;

        event_handler::push_event( e );
    }

    static void
    window_size_callback( GLFWwindow* p_window, int width, int height )
    {
        event e;
        e.window_resize.width = static_cast<uint32_t>( width );
        e.window_resize.height = static_cast<uint32_t>( height );
        e.event_type = event::type::window_resized;

        event_handler::push_event( e );
    }

    static void
    frame_buffer_size_callback( GLFWwindow* p_window, int width, int height )
    {
        event e;
        e.frame_buffer_resize.width = static_cast<uint32_t>( width );
        e.frame_buffer_resize.height = static_cast<uint32_t>( height );
        e.event_type = event::type::frame_buffer_resized;

        event_handler::push_event( e );
    }

    static void
    key_callback( GLFWwindow* p_window, int key, int scancode, int action, int mode )
    {
        event e;
        e.key.key = static_cast<io::keyboard::key>( key );

        if( action == GLFW_PRESS || action == GLFW_REPEAT )
            e.event_type = event::type::key_pressed;
        else if( action == GLFW_RELEASE )
            e.event_type = event::type::key_released;

        event_handler::push_event( e );
    }

    static void
    mouse_pos_callback( GLFWwindow* p_window, double x, double y )
    {
        event e;
        e.mouse_move.x = x;
        e.mouse_move.y = y;
        e.event_type = event::type::mouse_moved;

        event_handler::push_event( e );
    }

    static void
    mouse_button_callback( GLFWwindow* p_window, int button, int action, int mode )
    {
        event e;
        e.mouse_button.button = static_cast<io::mouse::button>( button );

        if( action == GLFW_PRESS || action == GLFW_REPEAT )
            e.event_type = event::type::mouse_button_pressed;
        else if( action == GLFW_RELEASE )
            e.event_type = event::type::mouse_button_released;

        event_handler::push_event( e );
    }
};

#endif //PROJEKT_GLFW_CALLBACKS_H
