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
    ~game( ) = default;

    game& operator=( const game& ) = delete;
    game& operator=( game&& ) = delete;

    void run();

private:
    void handle_input( event& e );
    void update( float delta_time );
    void render();

private:
    window& window_;

    vk::core::shader_module vertex_shader_;
    vk::core::shader_module fragment_shader_;

    renderer renderer_;

    uint32_t frame_count_ = 0;

    float time_passed = 0;
    int frames_passed = 0;

    const std::vector<vk::graphics::vertex> vertices = {
            { { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } },
            { {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f } },
            { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } },
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f } },
    };

    const std::vector<std::uint16_t> indices_ = {
            0, 1, 2, 0, 2, 3,
    };
};

#endif //PROJEKT_GAME_H
