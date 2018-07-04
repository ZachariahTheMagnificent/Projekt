/*!
 *
 */

#ifndef PROJEKT_EVENT_H
#define PROJEKT_EVENT_H

#include <cstdint>

struct event
{
    struct window_move_event
    {
        std::int32_t x;
        std::int32_t y;
    };

    struct window_resize_event
    {
        std::uint32_t width;
        std::uint32_t height;
    };

    enum class type : std::int32_t
    {
        null,

        window_moved,
        window_resized,

        count
    };

    union
    {
        window_move_event window_move;
        window_resize_event window_resize;
    };

    type event_type = type::null;
};

#endif //PROJEKT_EVENT_H
