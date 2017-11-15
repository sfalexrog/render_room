#include <SDL.h>

int main(int argc, char** argv)
{
    SDL_Window *w = SDL_CreateWindow("CMC Raymarching", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540, SDL_WINDOW_OPENGL);

    SDL_Event e;
    bool done = false;
    while (!done)
    {
        while(SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;
            }

        }
    }

    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}