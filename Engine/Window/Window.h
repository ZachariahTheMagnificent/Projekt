/*!
 *
 */

#ifndef PROJEKT_WINDOW_H
#define PROJEKT_WINDOW_H

#include <cstdint>
#include <string>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#ifdef NDEBUG
static constexpr bool enable_validation_layers = false;
#else
static constexpr bool enable_validation_layers = true;
#endif

class Window
{
public:
    Window( std::uint32_t width, std::uint32_t height, const std::string& title );
    Window( std::uint32_t x_pos, std::uint32_t y_pos, std::uint32_t width, std::uint32_t height, const std::string& title );
    Window( const Window& ) = delete;
    Window( Window&& ) = delete;
    ~Window( );

    Window& operator=( const Window& ) = delete;
    Window& operator=( Window&& ) = delete;

    void poll_event();

    bool is_open() const;

    VkSurfaceKHR create_surface( const VkInstance& instance_handle ) const;

    std::vector<const char*> get_required_extensions() const;

    const std::uint32_t get_width() const;
    const std::uint32_t get_height() const;
    const std::string& get_title() const;

private:
    void set_up();

private:
    std::uint32_t x_pos_    = 50;
    std::uint32_t y_pos_    = 50;
    std::uint32_t width_    = 0;
    std::uint32_t height_   = 0;

    const std::string title_;

    GLFWwindow* p_window_;
};

#endif //PROJEKT_WINDOW_H
