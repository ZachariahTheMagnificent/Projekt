/*!
 * @brief An exception class to handle vulkan related errors
 * based on the Exception class.
 */

#ifndef PROJEKT_VULKANEXCEPTION_H
#define PROJEKT_VULKANEXCEPTION_H

#include "Exception.h"

class VulkanException : public Exception
{
public:
    VulkanException( const std::string& message, const std::string& file, const std::uint64_t& line )
        :
        Exception( message, file, line )
    { }
};

#endif //PROJEKT_VULKANEXCEPTION_H
