/*!
 * @brief An exception class to handle glfw related errors
 * based on the Exception class.
 */

#ifndef PROJEKT_GLFWEXCEPTION_H
#define PROJEKT_GLFWEXCEPTION_H

#include "Exception.h"

class GlfwException : public Exception
{
public:
    GlfwException( const std::string& message, const std::string file, const std::uint64_t& line )
        :
        Exception( message, file, line )
    { }
};

#endif //PROJEKT_GLFWEXCEPTION_H
