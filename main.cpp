#include <iostream>

#include "Engine/Utils/Exception/Exception.h"
#include "Engine/Utils/Exception/VulkanException.h"
#include "Engine/Utils/Exception/GlfwException.h"
#include "Engine/Utils/File/Write.h"
#include "Engine/Window/Window.h"

#include "Game/Game.h"

int main( )
{
    std::string error_log_file = "error_log.txt";
    std::string error_warning = "Error has been found and logged in " + error_log_file;

    try
    {
        Window window( 100, 100, 1280, 720, "Projekt - Vulkan" );

        try
        {
            Game game( window );
            game.run();
        }
        catch( GlfwException& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "GLFW Exception caught at game creation and runtime.\nLocation: " + e.get_file()
                                        + ".\nLine: " + std::to_string( e.get_line() )
                                        + "\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( VulkanException& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "Vulkan Exception caught at game creation and runtime.\nLocation: " + e.get_file()
                                        + ".\nLine: " + std::to_string( e.get_line() )
                                        + "\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( Exception& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "Engine Exception caught at game creation and runtime.\nLocation: " + e.get_file()
                                        + ".\nLine: " + std::to_string( e.get_line() )
                                        + "\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( std::exception& e )
        {
            std::cerr << error_warning << std::endl;

            std::string what_str( e.what() );
            std::string error_message = "STL Exception caugh at game creation and runtime.\n\n" + what_str;

            write_to_file( error_log_file, error_message );
        }
        catch( ... )
        {
            std::cerr << error_warning << std::endl;

            std::string error_message = "Major fuck up caught at game creation and runtime.\n\nUnhandled unknown exception";

            write_to_file( error_log_file, error_message );
        }
    }
    catch( GlfwException& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "GLFW Exception caught at window creation.\nLocation: " + e.get_file()
                                    + ".\nLine: " + std::to_string( e.get_line() )
                                    + "\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( VulkanException& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "Vulkan Exception caught at window creation.\nLocation: " + e.get_file()
                                    + ".\nLine: " + std::to_string( e.get_line() )
                                    + "\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( Exception& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "Engine Exception caught at window creation.\nLocation: " + e.get_file()
                                    + ".\nLine: " + std::to_string( e.get_line() )
                                    + "\n\n" + what_str;

        write_to_file( error_log_file, error_message );
    }
    catch( std::exception& e )
    {
        std::cerr << error_warning << std::endl;

        std::string what_str( e.what() );
        std::string error_message = "STL Exception caugh at window creation.\n\n" + what_str;

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