/*!
 *
 */

#ifndef PROJEKT_KEYBOARD_H
#define PROJEKT_KEYBOARD_H

#include <cstdint>

namespace io
{
    struct keyboard
    {
        enum class key : std::int32_t
        {
            unknown = -1,
            menu = 348,
            last = 348,

            space = 32,
            escape = 256,
            enter = 257,
            tab = 258,
            backspace = 259,
            insert = 260,
            right = 262,
            left = 263,
            down = 264,
            up = 265,
            page_up = 266,
            page_down = 267,
            home = 268,
            end = 269,
            caps_lock = 280,
            scroll_lock = 281,
            num_lock = 282,
            print_screen = 283,
            pause = 284,
            left_shift = 340,
            left_control = 341,
            left_alt = 342,
            left_super = 343,
            right_shift = 344,
            right_control = 345,
            right_alt = 346,
            right_super = 347,
            apostrophe = 39,       /* ' */
            comma = 44,            /* , */
            period = 46,           /* . */
            minus = 45,            /* - */
            equal = 61,            /* = */
            slash = 47,            /* / */
            semicolon = 59,        /* ; */
            left_bracket = 91,     /* [ */
            backslash = 92,        /* \ */
            right_bracket = 93,    /* ] */
            grave_accent = 96,     /* ` */
            world_1 = 161,         /* non-US #1 */
            world_2 = 162,         /* non-US #2 */

            f1 = 290,
            f2 = 291,
            f3 = 292,
            f4 = 293,
            f5 = 294,
            f6 = 295,
            f7 = 296,
            f8 = 297,
            f9 = 298,
            f10 = 299,
            f11 = 300,
            f12 = 301,
            f13 = 302,
            f14 = 303,
            f15 = 304,
            f16 = 305,
            f17 = 306,
            f18 = 307,
            f19 = 308,
            f20 = 309,
            f21 = 310,
            f22 = 311,
            f23 = 312,
            f24 = 313,
            f25 = 314,

            kp_0 = 320,
            kp_1 = 321,
            kp_2 = 322,
            kp_3 = 323,
            kp_4 = 324,
            kp_5 = 325,
            kp_6 = 326,
            kp_7 = 327,
            kp_8 = 328,
            kp_9 = 329,
            kp_decimal = 330,
            kp_divide = 331,
            kp_multiply = 332,
            kp_substract = 333,
            kp_add = 334,
            kp_enter = 335,
            kp_equal = 336,

            num_0 = 48,
            num_1 = 49,
            num_2 = 50,
            num_3 = 51,
            num_4 = 52,
            num_5 = 53,
            num_6 = 54,
            num_7 = 55,
            num_8 = 56,
            num_9 = 57,

            A = 65,
            B = 66,
            C = 67,
            D = 68,
            E = 69,
            F = 70,
            G = 71,
            H = 72,
            I = 73,
            J = 74,
            K = 75,
            L = 76,
            M = 77,
            N = 78,
            O = 79,
            P = 80,
            Q = 81,
            R = 82,
            S = 83,
            T = 84,
            U = 85,
            V = 86,
            W = 87,
            X = 88,
            Y = 89,
            Z = 90,
        };
    };
}


#endif //PROJEKT_KEYBOARD_H
