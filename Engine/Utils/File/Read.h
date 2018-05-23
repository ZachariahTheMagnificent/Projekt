/*!
 * @brief set of functions to read from a file.
 */

#ifndef PROJEKT_READ_H
#define PROJEKT_READ_H

#include <string>
#include <fstream>

#include "../Exception/Exception.h"

inline std::string read_from_file( const std::string& filepath )
{
    std::ifstream file( filepath );
    std::string str;

    if( !file.tellg() < 0 )
        throw Exception{ "Error loading file at location: " + filepath + ".", __FILE__, __LINE__ };
    else if( !file.good() )
        throw Exception{ "Error reading file: " + filepath + ".", __FILE__, __LINE__ };

    char c;
    while( file.get( c ) )
    {
        str.push_back( c );
    }

    return str;
}

inline std::string read_from_binary_file( const std::string filepath )
{
    std::ifstream file( filepath , std::ios::binary );
    std::string str;

    if( !file.tellg() < 0 )
        throw Exception{ "Error loading file at location: " + filepath + ".", __FILE__, __LINE__ };
    else if( !file.good() )
        throw Exception{ "Error reading file: " + filepath + ".", __FILE__, __LINE__ };

    char c;
    while( file.get( c ) )
    {
        str.push_back( c );
    }

    return str;
}

#endif //PROJEKT_READ_H
