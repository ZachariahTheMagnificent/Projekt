/*!
 *
 */

#ifndef PROJEKT_UNIFORM_BUFFER_OBJECT_H
#define PROJEKT_UNIFORM_BUFFER_OBJECT_H

#include <glm/glm.hpp>

namespace vk
{
    namespace graphics
    {
        struct uniform_buffer_object
        {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
        };
    }
}

#endif //PROJEKT_UNIFORM_BUFFER_OBJECT_H
