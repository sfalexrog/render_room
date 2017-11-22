#ifndef CMC_RAYMARCH_CONFIG_H
#define CMC_RAYMARCH_CONFIG_H

#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace cmcray
{
    namespace Config
    {

#define OPT(type, name, defValue) extern type name;
#include "Config.inl"

#undef OPT

        void init(int argc, char** argv);

    }
}

#endif //CMC_RAYMARCH_CONFIG_H
