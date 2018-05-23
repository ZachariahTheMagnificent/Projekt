/*!
 *
 */

#ifndef PROJEKT_GAME_H
#define PROJEKT_GAME_H

#include "../Engine/Graphics/Renderer.h"
#include "../Engine/Window/Window.h"

class Game
{
public:
    explicit Game( Window& window );
    Game( const Game& ) = delete;
    Game( Game&& ) = delete;
    ~Game( );

    Game& operator=( const Game& ) = delete;
    Game& operator=( Game&& ) = delete;

    void run();

private:
    void handle_input();
    void update( float delta_time );
    void render();

private:
    Window& window_;

    Renderer renderer_;
};

#endif //PROJEKT_GAME_H
