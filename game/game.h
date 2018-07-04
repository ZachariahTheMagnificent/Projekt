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

    void calculate_fps( float delta_time, uint32_t interval );

private:
    window& window_;

    vk::core::shader_module vertex_shader_;
    vk::core::shader_module fragment_shader_;

    renderer renderer_;

    uint32_t frame_count_ = 0;

    const std::vector<vk::graphics::vertex> vertices = {
            { { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
    };

    const std::vector<std::uint16_t> indices_ = {
            0, 1, 3, 1, 2, 3
    };
};

#endif //PROJEKT_GAME_H
