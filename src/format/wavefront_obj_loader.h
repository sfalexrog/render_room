//
// Created by sf on 18/11/17.
//

#ifndef CMC_RAYMARCH_WAVEFRONT_OBJ_LOADER_H
#define CMC_RAYMARCH_WAVEFRONT_OBJ_LOADER_H

#include "mesh.h"
#include <string>

namespace cmcray
{
    namespace Loader
    {
        Mesh loadObj(const std::string& fileName);
    }
}

#endif //CMC_RAYMARCH_WAVEFRONT_OBJ_LOADER_H
