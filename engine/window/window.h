/*!
 *
 */

#ifndef PROJEKT_WINDOW_H
#define PROJEKT_WINDOW_H

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include "event/event_handler.h"

#ifdef NDEBUG
static constexpr bool enable_validation_layers = false;
#else
static constexpr bool enable_validation_layers = true;
#endif

class window
{
public:
    window( std::uint32_t width, std::uint32_t height, const std::string& title );
    window( std::uint32_t x_pos, std::uint32_t y_pos, std::uint32_t width, std::uint32_t height, const std::string& title );
    window( const window& ) = delete;
    window( window&& ) = delete;
    ~window( );

    window& operator=( const window& ) = delete;
    window& operator=( window&& ) = delete;

    void poll_event();
    void handle_event( event& e );

    bool is_open() const;

    VkSurfaceKHR create_surface( const VkInstance& instance_handle ) const;

    std::vector<const char*> get_required_extensions() const;

    const std::uint32_t get_width() const;
    const std::uint32_t get_height() const;
    const std::string& get_title() const;

private:
    void set_up();

private:
    void set_window_resize_callback( GLFWwindowsizefun window_resize_callback );
    void set_window_position_callback( GLFWwindowposfun window_position_callback );

private:
    std::uint32_t x_pos_    = 50;
    std::uint32_t y_pos_    = 50;
    std::uint32_t width_    = 0;
    std::uint32_t height_   = 0;

    const std::string title_;

    GLFWwindow* p_window_;
};

#endif //PROJEKT_WINDOW_H
