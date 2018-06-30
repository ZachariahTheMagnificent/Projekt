/*!
 *
 */

#ifndef PROJEKT_VERTEXBUFFER_H
#define PROJEKT_VERTEXBUFFER_H

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct vertex
{
    glm::vec3 position;
    glm::vec3 colour;

    static VkVertexInputBindingDescription get_binding_description()
    {
        VkVertexInputBindingDescription binding_description = {};
        binding_description.binding = 0;
        binding_description.stride = sizeof( vertex );
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions = {};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = static_cast<uint32_t>( offsetof( vertex, position ) );

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = static_cast<uint32_t>( offsetof( vertex, colour ) );

        return attribute_descriptions;
    };
};

#endif //PROJEKT_VERTEXBUFFER_H
