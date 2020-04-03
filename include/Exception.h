#ifndef PROCESS_EXCEPTION_H
#define PROCESS_EXCEPTION_H

#include <string>
#include <exception>

class StreamError : public std::exception
{
private:
    std::string m_error;

public:
    StreamError(std::string error)
            : m_error("StreamError: " + error)
    {}

    const char *what() const noexcept
    {
        return m_error.c_str();
    }
};

class CreateError : public std::exception
{
private:
    std::string m_error;

public:
    CreateError(std::string error)
            : m_error("CreateError: " + error)
    {}

    const char *what() const noexcept
    {
        return m_error.c_str();
    }
};

#endif //PROCESS_EXCEPTION_H
