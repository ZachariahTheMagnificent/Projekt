/*!
 *
 */

#include <chrono>
#include <iostream>

#include "game.h"

game::game( window& window )
    :
    window_( window ),
    renderer_( window_ )
{

}

game::~game( )
{

}

void
game::run( )
{
    auto time_point = std::chrono::steady_clock::now( );
    float max_dt = 1.0f / 20.0f;

    while( window_.is_open( ) )
    {
        window_.poll_event();

        float dt;
        {
            const auto new_time_point = std::chrono::steady_clock::now( );
            dt = std::chrono::duration<float>( new_time_point - time_point ).count( );
            time_point = new_time_point;
        }
        dt = std::min( dt, max_dt );

        float fps = ( 1.0f / dt );

        //std::cout << fps << "\n";

        handle_input();
        update( dt );
        render( );
    }
}

void
game::handle_input( )
{

}

void
game::update( float delta_time )
{

}

void
game::render( )
{
    renderer_.draw_frame();
}