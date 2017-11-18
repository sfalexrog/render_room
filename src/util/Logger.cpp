#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "Logger.h"
#include "InteractiveLog.h"

namespace cmcray
{
    namespace InteractiveLog
    {
        const size_t MAX_INTERACTIVE_LOG_LENGTH = 1024;

        std::vector<std::string> circularLog;

        size_t numEntries = 0;
        size_t offsetEntries = 0;
        Logger::LogLevel verbosity;

        std::string empty;

        void initLog()
        {
            empty = "";
            circularLog.clear();
            offsetEntries = 0;
            numEntries = 0;
            circularLog.reserve(MAX_INTERACTIVE_LOG_LENGTH);
        }

        size_t getLogSize()
        {
            return circularLog.size();
        }

        const std::string& getLogLineAt(size_t position)
        {
            if (position > numEntries) return empty;
            return circularLog[(position + offsetEntries) % MAX_INTERACTIVE_LOG_LENGTH];
        }

        void clearLog()
        {
            initLog();
        }

        void setMinVerbosity(Logger::LogLevel minLogLevel)
        {
            verbosity = minLogLevel;
        }

        void addLine(const std::string& logLine)
        {
            if (circularLog.size() < MAX_INTERACTIVE_LOG_LENGTH)
            {
                circularLog.push_back(logLine);
                numEntries++;
            }
            else
            {
                circularLog[(numEntries + offsetEntries) % MAX_INTERACTIVE_LOG_LENGTH] = logLine;
                offsetEntries++; offsetEntries %= MAX_INTERACTIVE_LOG_LENGTH;
            }
        }

    }
}

namespace cmcray
{
    void Logger::_write(const std::string &logLine)
    {
        if (_ll >= InteractiveLog::verbosity)
        {
            InteractiveLog::addLine(logLine);
        }

        if (_ll < LogLevel::ERROR)
        {
            std::cout << logLine << std::endl;
        }
        else
        {
            std::cerr << logLine << std::endl;
        }

    }

}