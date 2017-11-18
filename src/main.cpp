#include <SDL.h>
#include <glbinding/gl33core/gl.h>

#include <imgui.h>
#include "ui/imgui_impl_sdl_gl3.h"
#include <glbinding/Binding.h>
#include <glbinding/gl43core/gl.h>
#include <iostream>

#include "util/Logger.h"
#include "ui/LogWindow.h"
#include "ui/MapWindow.h"

#include "format/wavefront_obj_loader.h"

int main(int argc, char** argv)
{
    SDL_Window *w = SDL_CreateWindow("CMC Raymarching", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 800, SDL_WINDOW_OPENGL);

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
    auto room = cmcray::Loader::loadObj("cmc_rooms/room01.obj");
    Log(INFO) << "Loading room took " << (SDL_GetTicks() - load_start) << " ms";

    bool done = false;
    auto start_time = SDL_GetTicks();
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
            }
        }
        ImGui_ImplSdlGL3_NewFrame(w);

        ImGui::Begin("Ray Marching Controls");
        {
            ImGui::Text("Hello from ImGui!");
        }
        ImGui::End();

        cmcray::LogWindow::draw();
        cmcray::MapWindow::draw(room);

        gl::glClearColor(0, 0, 0, 255);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        SDL_GL_SwapWindow(w);
    }

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}