/*!
 *
 */

#ifndef PROJEKT_EVENT_H
#define PROJEKT_EVENT_H

#include <cstdint>

#include "../io/mouse.h"
#include "../io/keyboard.h"

struct event
{
    struct mouse_move_event
    {
        double x;
        double y;
    };

    struct mouse_button_event
    {
        io::mouse::button button;
    };

    struct keybord_key_event
    {
        io::keyboard::key key;
    };

    struct window_move_event
    {
        std::uint32_t x;
        std::uint32_t y;
    };

    struct window_resize_event
    {
        std::uint32_t width;
        std::uint32_t height;
    };

    struct frame_buffer_resize_event
    {
        std::uint32_t width;
        std::uint32_t height;
    };

    enum class type : std::int32_t
    {
        null,

        window_moved,
        window_resized,

        frame_buffer_resized,

        mouse_moved,
        mouse_button_pressed,
        mouse_button_released,

        key_pressed,
        key_released,

        count
    };

    union
    {
        window_move_event window_move;
        window_resize_event window_resize;

        frame_buffer_resize_event frame_buffer_resize;

        mouse_move_event mouse_move;
        mouse_button_event mouse_button;

        keybord_key_event key;
    };

    type event_type = type::null;
};

#endif //PROJEKT_EVENT_H
