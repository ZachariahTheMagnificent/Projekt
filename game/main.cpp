#include <iostream>

#include "../engine/utils/exception/exception.h"
#include "../engine/utils/exception/vulkan_exception.h"
#include "../engine/utils/exception/glfw_exception.h"
#include "../engine/utils/file_io/write.h"
#include "../engine/window/window.h"

#include "game.h"

int main( )
{
    std::string error_log_file = "error_log.txt";
    std::string error_warning = "Error has been found and logged in " + error_log_file;

    try
    {
        window window( 100, 100, 1280, 720, "Projekt - vulkan" );

        try
        {
            game game( window );
            game.run();
        }
        catch( glfw_exception& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "GLFW exception caught at game creation and runtime.\nLocation: " + e.get_file()
                                        + ".\nLine: " + std::to_string( e.get_line() )
                                        + "\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( vulkan_exception& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "vulkan exception caught at game creation and runtime.\nLocation: " + e.get_file()
                                        + ".\nLine: " + std::to_string( e.get_line() )
                                        + "\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( exception& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "engine exception caught at game creation and runtime.\nLocation: " + e.get_file()
                                        + ".\nLine: " + std::to_string( e.get_line() )
                                        + "\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( std::exception& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "STL exception caugh at game creation and runtime.\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( ... )
        {
            std::cerr << error_warning << std::endl;

            std::string error_message = "Major fuck up caught at game creation and runtime.\n\nUnhandled unknown exception";

            write_to_file( error_log_file, error_message );
        }
    }
    catch( glfw_exception& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "GLFW exception caught at window creation.\nLocation: " + e.get_file()
                                    + ".\nLine: " + std::to_string( e.get_line() )
                                    + "\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( vulkan_exception& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "vulkan exception caught at window creation.\nLocation: " + e.get_file()
                                    + ".\nLine: " + std::to_string( e.get_line() )
                                    + "\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( exception& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "engine exception caught at window creation.\nLocation: " + e.get_file()
                                    + ".\nLine: " + std::to_string( e.get_line() )
                                    + "\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( std::exception& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "STL exception caugh at window creation.\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( ... )
    {
        std::cerr << error_warning << std::endl;

        std::string error_message = "Major fuck up caught at window creation.\n\nUnhandled unknown exception";

        write_to_file( error_log_file, error_message );
    }

    return 0;
}