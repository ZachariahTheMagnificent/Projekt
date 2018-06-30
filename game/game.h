/*!
 *
 */

#ifndef PROJEKT_GAME_H
#define PROJEKT_GAME_H

#include "../engine/graphics/renderer.h"
#include "../engine/window/window.h"

class game
{
public:
    explicit game( window& window );
    game( const game& ) = delete;
    game( game&& ) = delete;
    ~game( );

    game& operator=( const game& ) = delete;
    game& operator=( game&& ) = delete;

    void run();

private:
    void handle_input();
    void update( float delta_time );
    void render();

private:
    window& window_;

    renderer renderer_;
};

#endif //PROJEKT_GAME_H
