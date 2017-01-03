#ifndef BS_EXCEPTION_H_
#define BS_EXCEPTION_H_

#include <exception>

namespace BS
{
    class BSExceptionBadType: public std::exception
    {
        const std::string msg;
    public:
        BSExceptionBadType(int stype) throw() :
            msg(std::string("Unexpected number type: ") + std::to_string(stype))
        {}
        virtual ~BSExceptionBadType(void) throw() {}
        virtual const char *what(void) const throw() {
            return msg.c_str();
        }
    };

    class BSExceptionNoOutputAvailable: public std::exception
    {
        const std::string msg;
    public:
        BSExceptionNoOutputAvailable(void) throw():
            msg("No output is available as not yet generated"){}
        virtual ~BSExceptionNoOutputAvailable(void) throw() {}
        virtual const char *what(void) const throw() {
            return msg.c_str();
        }
    };

    class BSExceptionOutOfRange: public std::exception
    {
        const std::string msg;
    public:
        BSExceptionOutOfRange(int index) throw():
            msg(std::string("Index '") +
                    std::to_string(index) +
                    std::string("' is out of range.")){}
        virtual ~BSExceptionOutOfRange(void) throw() {}
        virtual const char *what(void) const throw() {
            return msg.c_str();
        }
    };
}

#endif //BS_EXCEPTION_H_
