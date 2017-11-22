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
OPT(glm::vec3, cameraPos, glm::vec3{0})

