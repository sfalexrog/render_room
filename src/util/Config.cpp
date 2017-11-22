#include "Config.h"
#include <rapidjson/document.h>

namespace cmcray
{
    namespace Config
    {

#define OPT(type, name, defValue) type name = defValue;
#include "Config.inl"
#undef OPT

        void init(int argc, char** argv)
        {

            
        }

    }
}

