//
// Created by sf on 18/11/17.
//

#ifndef CMC_RAYMARCH_MAPWINDOW_H
#define CMC_RAYMARCH_MAPWINDOW_H

#include "format/mesh.h"
#include "render/camera.h"

namespace cmcray
{
    namespace MapWindow
    {
        // Draw a top-down view of the mesh
        void draw(const Mesh& mesh, const Camera& camera);
    }
}

#endif //CMC_RAYMARCH_MAPWINDOW_H
