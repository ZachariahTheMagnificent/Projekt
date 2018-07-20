/*!
 *
 */

#ifndef PROJEKT_MOUSE_H
#define PROJEKT_MOUSE_H

#include <cstdint>

namespace io
{
    struct mouse
    {
        enum class button : std::uint32_t
        {
            left_click,
            right_click,
            scroll_click,
            button_4,
            button_5,
            button_6,
            button_7,
            button_8
        };
    };
}

#endif //PROJEKT_MOUSE_H
