/*!
 * @brief An exception class to handle vulkan related errors
 * based on the Exception class.
 */

#ifndef PROJEKT_VULKANEXCEPTION_H
#define PROJEKT_VULKANEXCEPTION_H

#include "exception.h"

class vulkan_exception : public exception
{
public:
    vulkan_exception( const std::string& message, const std::string& file, const std::uint64_t& line )
        :
        exception( message, file, line )
    { }
};

#endif //PROJEKT_VULKANEXCEPTION_H
