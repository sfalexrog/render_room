#include <SDL.h>
#include <glbinding/gl33core/gl.h>

#include <unistd.h>
#include <cstdio>

#include <imgui.h>
#include "ui/imgui_impl_sdl_gl3.h"
#include <glbinding/Binding.h>
#include <glbinding/gl43core/gl.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "util/Logger.h"
#include "ui/LogWindow.h"
#include "ui/MapWindow.h"
#include "render/camera.h"

#include "util/InteractiveLog.h"

#include "format/wavefront_obj_loader.h"

#include "render/shader.h"
#include "render/render.h"

#include "render/voxel_map.h"
#include "render/compute.h"

#include "util/Config.h"

int main(int argc, char** argv)
{
    cmcray::Config::init(argc, argv);

    SDL_Window *w = SDL_CreateWindow("CMC Raymarching",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     cmcray::Config::screenX,
                                     cmcray::Config::screenY,
                                     SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GLContext ctx = SDL_GL_CreateContext(w);

    glbinding::Binding::initialize();

    ImGui_ImplSdlGL3_Init(w);

    Log(DEBUG) << "VERSION: " << gl::glGetString(gl::GL_VERSION);
    Log(DEBUG) << "RENDERER: " << gl::glGetString(gl::GL_RENDERER);
    gl::GLint numExtensions;
    gl::glGetIntegerv(gl::GL_NUM_EXTENSIONS, &numExtensions);
    Log(DEBUG) << "EXTENSIONS: ";
    for (gl::GLint i = 0; i < numExtensions; ++i)
    {
        Log(DEBUG) << gl::glGetStringi(gl::GL_EXTENSIONS, i);
    }
    Log(DEBUG) << "Max compute group count: ";
    int maxCX = 0, maxCY = 0, maxCZ = 0;
    gl::glGetIntegeri_v(gl::GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxCX);
    gl::glGetIntegeri_v(gl::GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxCY);
    gl::glGetIntegeri_v(gl::GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxCZ);
    Log(DEBUG) << maxCX << "x" << maxCY << "x" << maxCZ << std::endl;
    int maxComputeInvocations = 0;
    gl::glGetIntegerv(gl::GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxComputeInvocations);
    Log(DEBUG) << "Total invocations: " << maxComputeInvocations << std::endl;

    auto load_start = SDL_GetTicks();
    auto room = cmcray::Loader::loadObj(cmcray::Config::sourceFile);
    Log(INFO) << "Loading room took " << (SDL_GetTicks() - load_start) << " ms";

    cmcray::InteractiveLog::setMinVerbosity(cmcray::Logger::LogLevel::VERBOSE);
    cmcray::Logger::minLogLevel() = cmcray::Logger::VERBOSE;

    Log(INFO) << "Loading shaders";

    auto renderShader = cmcray::Shader();
    auto computeShader = cmcray::Shader();

    {
        const char* vertShaderFile = "shaders/polyrender.vert";
        struct stat sb;
        if (!stat(vertShaderFile, &sb))
        {
            std::vector<char> vertShaderSrc(sb.st_size + 1, '\0');
            std::ifstream in(vertShaderFile);
            in.read(vertShaderSrc.data(), vertShaderSrc.size());
            renderShader.source(vertShaderSrc.data(), cmcray::Shader::Type::Vertex);
        }
        const char* fragShaderFile = "shaders/polyrender.frag";
        if (!stat(fragShaderFile, &sb))
        {
            std::vector<char> fragShaderSrc(sb.st_size + 1, '\0');
            std::ifstream in(fragShaderFile);
            in.read(fragShaderSrc.data(), fragShaderSrc.size());
            renderShader.source(fragShaderSrc.data(), cmcray::Shader::Type::Fragment);
        }
        renderShader.link();

        const char* compShaderFile = "shaders/raymarch_dumb.comp";
        if (!stat(compShaderFile, &sb))
        {
            std::vector<char> compShaderSrc(sb.st_size + 1, '\0');
            std::ifstream in(compShaderFile);
            in.read(compShaderSrc.data(), compShaderSrc.size());
            computeShader.source(compShaderSrc.data(), cmcray::Shader::Type::Compute);
            computeShader.link();
        }
    }

    auto &cam = cmcray::getDefaultCamera();
    cmcray::Renderer::init();
    cmcray::Compute::init();

    Log(INFO) << "Adding room to scene";
    cmcray::Renderer::addToScene(room);

    Log(INFO) << "Adding room to compute";
    cmcray::Compute::addToCompute(room);


    Log(INFO) << "Creating storage for compute results";

    auto voxmap = cmcray::VoxelMap(cmcray::Config::voxelMapDimensions);

    Log(INFO) << "Dispatching compute";

    cmcray::Compute::compute(computeShader, voxmap);

    bool done = false;
    auto start_time = SDL_GetTicks();
    float shift = 0, rot = 0;

    bool didDumpImages = false;
    bool spacePressed = false;

    while (!done)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&e);
            switch (e.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                            shift = 1;
                            break;
                        case SDL_SCANCODE_DOWN:
                            shift = -1;
                            break;
                        case SDL_SCANCODE_LEFT:
                            rot = 1;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            rot = -1;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_DOWN:
                            shift = 0;
                            break;
                        case SDL_SCANCODE_LEFT:
                        case SDL_SCANCODE_RIGHT:
                            rot = 0;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }

#if 0
        if (cmcray::Compute::finished() && !didDumpImages)
        {
            Log(INFO) << "Dumping images";

            auto dump_start = SDL_GetTicks();
            voxmap.dumpImages("img/dump");
            Log(INFO) << "Dumping took " << SDL_GetTicks() - dump_start << " ms";
            didDumpImages = true;
        }
#endif
        auto frame_time = SDL_GetTicks() - start_time;

        glm::vec3 camDirection = glm::vec3{cos(cam.rotation.y), 0.0f, -sin(cam.rotation.y)};
        cam.position += camDirection * (shift * frame_time * 2.0f);
        cam.rotation.y += rot * frame_time * (glm::pi<float>() / 1024.0f);

        start_time = SDL_GetTicks();
        ImGui_ImplSdlGL3_NewFrame(w);

        ImGui::Begin("Ray Marching Controls");
        {
            ImGui::Text("Frame time: %d (%f fps)", frame_time, 1000.0f/frame_time);
        }
        ImGui::End();

        //cmcray::Compute::compute(computeShader, voxmap);

        cmcray::LogWindow::draw();
        cmcray::MapWindow::draw(room, cam, voxmap);

        gl::glClearColor(0, 0, 0, 255);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        cmcray::Renderer::render(renderShader, cam);

        ImGui::Render();
        SDL_GL_SwapWindow(w);
    }

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}