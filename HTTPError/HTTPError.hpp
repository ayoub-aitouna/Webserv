#ifndef HTTPERROR
#define HTTPERROR
#include <exception>

class HTTPError : public std::exception
{
public:
    int statusCode;
    HTTPError(int statusCode) : statusCode(statusCode) {}

public:
    const char *what() const throw()
    {
        return "HTTPError";
    }
};

#endif