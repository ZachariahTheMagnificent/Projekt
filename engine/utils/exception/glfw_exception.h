/*!
 * @brief An exception class to handle glfw related errors
 * based on the Exception class.
 */

#ifndef PROJEKT_GLFWEXCEPTION_H
#define PROJEKT_GLFWEXCEPTION_H

#include "exception.h"

class glfw_exception : public exception
{
public:
    glfw_exception( const std::string& message, const std::string file, const std::uint64_t& line )
        :
        exception( message, file, line )
    { }
};

#endif //PROJEKT_GLFWEXCEPTION_H
