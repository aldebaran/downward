#ifndef UTILS_EXCEPTIONS_H
#define UTILS_EXCEPTIONS_H

#include <stdexcept>

namespace utils {
// Base class for custom exception types.
class Exception: public std::exception {
public:
    virtual ~Exception() = default;
    virtual void print() const = 0;
    virtual const char* what() const noexcept = 0;
};
}

#endif
