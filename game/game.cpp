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
    renderer_.prepare_pipeline( "../game/shaders/vert.spv" , "../game/shaders/frag.spv" );
    renderer_.prepare_for_rendering( vertices, indices_ );
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

        time_passed += dt;
        frames_passed += 1;

        if( time_passed >= 0.1 )
        {
            window_.set_title( window_.get_title() + " : FPS - " + std::to_string( ( frames_passed / time_passed ) ) );

            time_passed = 0;
            frames_passed = 0;
        }

        auto events = event_handler::pull();
        if( !events.empty() )
        {
            for( auto& e : events )
            {
                window_.handle_event( e );
                renderer_.handle_event( e );

                handle_input( e );
            }
        }

        renderer_.prepare_frame( );

        update( dt );
        render( );

        renderer_.submit_frame( );
    }
}

void
game::handle_input( event& e )
{

}

void
game::update( float delta_time )
{
    renderer_.update( delta_time );
}

void
game::render( )
{

}
