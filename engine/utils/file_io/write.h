/*!
 * @brief a set of functions to write to a file.
 */

#ifndef PROJEKT_WRITE_H
#define PROJEKT_WRITE_H

#include <string>
#include <fstream>

#include "../exception/exception.h"

inline void
write_to_file( const std::string& filepath, const std::string& data )
{
    std::ofstream file( filepath );

    if( !file.good() )
        throw exception{ "Error finding file: " + filepath + ".", __FILE__, __LINE__ };

    file << data;
}

#endif //PROJEKT_WRITE_H
