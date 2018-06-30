/*!
 *  @brief  A basic exception that points to the location of the error.
 */

#ifndef PROJEKT_EXCEPTION_H
#define PROJEKT_EXCEPTION_H

#include <stdexcept>
#include <string>

class exception : public std::runtime_error
{
public:
    exception( const std::string& message, const std::string& file, const std::uint64_t& line )
        :
        runtime_error( message ),
        file_( file ),
        line_( line )
    { }

    const std::string& get_file() const
    {
        return file_;
    }

    const std::uint64_t get_line() const
    {
        return line_;
    }

private:
    const std::string file_;
    const std::uint64_t line_;
};

#endif //PROJEKT_EXCEPTION_H
