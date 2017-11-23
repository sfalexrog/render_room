/**
 * Options file: describes all currently available options
 * Format is:
 * OPT(type, name, default value)
 *
 * Add your own toValue and fromValue overloads for non-trivial types.
 *
 * Do not omit namespaces in class/type names.
 *
 * Note that the contents of this file will change dramatically over time.
 *
 */

// Screen width
OPT(int, screenX, 1600)
// Screen height
OPT(int, screenY, 800)
// Model to use
OPT(std::string, sourceFile, "cmc_rooms/room01.obj")
// Model transformations (applied at load)
OPT(glm::mat4, sourceTransform, glm::mat4{1.0f})
// Scale factor (how much GL units are in a metre)
OPT(float, scaleFactor, 1000.0f)
// Initial camera position
OPT(glm::vec3, cameraPos, glm::vec3{0.0f})
// Camera field-of-view
OPT(float, cameraFov, glm::pi<float>() / 2)
// Initial camera rotation
OPT(glm::vec2, cameraRotation, glm::vec2{0.0f})
// Near plane distance
OPT(float, cameraNear, 100.0f)
// Far plane distance
OPT(float, cameraFar, 10000.0f)
// Light intensity (pre-scale-factor)
OPT(float, lightIntensity, 50.0f)
