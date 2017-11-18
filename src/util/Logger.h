//
// Created by sf on 18/11/17.
//

#ifndef CMC_RAYMARCH_LOGGER_H
#define CMC_RAYMARCH_LOGGER_H

#include <vector>
#include <string>
#include <sstream>

namespace cmcray
{
    class Logger
    {
    public:
        enum LogLevel
        {
            VERBOSE = 0,
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            FATAL
        };
    private:
        std::stringstream _ss;
        LogLevel _ll;
        void _write(const std::string &logLine);

        const char* llStr(LogLevel l)
        {
            switch(l)
            {
                case LogLevel::VERBOSE:
                    return "[VERBOSE] ";
                case LogLevel::DEBUG:
                    return "[DEBUG] ";
                case LogLevel::INFO:
                    return "[INFO] ";
                case LogLevel::WARNING:
                    return "[WARNING] ";
                case LogLevel::ERROR:
                    return "[ERROR] ";
                case LogLevel::FATAL:
                    return "[FATAL] ";
                default:
                    return "[UNKNOWN!!] ";

            }
        }

    public:



        static LogLevel& minLogLevel()
        {
            static LogLevel minLogLevel = LogLevel::DEBUG;
            return minLogLevel;
        }

        Logger(LogLevel ll) : _ll(ll) { _ss << llStr(_ll); }
        ~Logger()
        {
            if (_ll >= minLogLevel())
            {
                _write(_ss.str());
            }
        }

        std::stringstream& get()
        {
            return _ss;
        }
    };
}

#define Log(LOGLEVEL) cmcray::Logger(cmcray::Logger::LOGLEVEL).get()

#endif //CMC_RAYMARCH_LOGGER_H
