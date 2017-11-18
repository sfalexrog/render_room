#ifndef CMC_RAYMARCH_INTERACTIVELOG_H
#define CMC_RAYMARCH_INTERACTIVELOG_H

#include <string>
#include "Logger.h"

namespace cmcray
{
    namespace InteractiveLog
    {
        size_t getLogSize();
        const std::string& getLogLineAt(size_t position);
        void clearLog();
        void setMinVerbosity(Logger::LogLevel minLogLevel);
    }
}

#endif //CMC_RAYMARCH_INTERACTIVELOG_H
