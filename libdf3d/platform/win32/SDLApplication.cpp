#include "df3d_pch.h"
#include "SDLApplication.h"

#include <SDL_image.h>
#include <base/Controller.h>

#if defined(__WIN32__)

namespace df3d { namespace platform {

SDLApplication::SDLApplication()
    : m_window(0), m_glContext(0)
{
}

SDLApplication::~SDLApplication()
{
}

bool SDLApplication::init(AppInitParams params)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        base::glog << "Could not initialize SDL" << base::logcritical;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // Does it work?
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    // FIXME:
    // This is renderer.init()
    unsigned int sdlInitFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (params.fullscreen)
        sdlInitFlags |= SDL_WINDOW_FULLSCREEN;

    m_window = SDL_CreateWindow("df3d application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        params.windowWidth, params.windowHeight, sdlInitFlags);
    if (!m_window)
    {
        base::glog << "Can not set video mode" << base::logcritical;
        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext)
    {
        base::glog << "Can not create OpenGL context:" << SDL_GetError() << base::logcritical;
        return false;
    }

    // SDL_Image init.
    int imgLibInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    auto imgLibInitialized = IMG_Init(imgLibInitFlags);
    if ((imgLibInitFlags & imgLibInitialized) != imgLibInitFlags)
    {
        base::glog << "Couldn't load image support library due to" << IMG_GetError() << base::logcritical;
        return false;
    }

    return true;
}

void SDLApplication::shutdown()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    IMG_Quit();
    SDL_Quit();
}

void SDLApplication::pollEvents()
{
    static SDL_Event sdlevent;

    while (SDL_PollEvent(&sdlevent))
    {
        g_engineController->dispatchAppEvent(&sdlevent);
    }
}

void SDLApplication::swapBuffers()
{
    // FIXME:
    // Create class RenderWindow with method swapBuffers
    SDL_GL_SwapWindow(m_window);
}

void SDLApplication::setTitle(const char *title)
{
    SDL_SetWindowTitle(m_window, title);
}

} }

#endif // __WIN32__