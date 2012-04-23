#ifndef DEBUG_H
#define DEBUG_H
#include <iostream>

class Logger
{
    public:
        Logger (std::ostream* os):out_stream(os)
        {
        };
        virtual ~Logger() {};

        template <typename T>
            inline Logger& Display(T thing)
            {
                *out_stream << thing;
                return *this;
            }

    private:
        std::ostream* out_stream;
};

template <typename T>
Logger& operator<< (Logger& logger, T thing) 
{
    return logger.Display(thing);
}

static Logger *myLogger;


#endif
