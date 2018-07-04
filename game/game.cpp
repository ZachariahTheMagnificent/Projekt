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
    renderer_.prepare_for_rendering( vertices, indices_ );
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
        float dt;
        {
            const auto new_time_point = std::chrono::steady_clock::now( );
            dt = std::chrono::duration<float>( new_time_point - time_point ).count( );
            time_point = new_time_point;
        }
        dt = std::min( dt, max_dt );

        float fps = ( 1.0f / dt );

        event e;
        while( window_.poll_event( e ) )
        {
            handle_input( e );
        }

        renderer_.prepare_frame( e );

        update( dt );
        render( );

        renderer_.submit_frame( e );
    }
}

void
game::handle_input( event& e )
{

}

void
game::update( float delta_time )
{

}

void
game::render( )
{

}
