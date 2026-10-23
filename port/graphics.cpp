#include "graphics.h"
#include <emscripten.h>
#include <cstring>

#include "errors.h"

extern SDL::Surface* backSurface;

void Graphics::Init() {
    // The port yields explicitly. Avoid a second Asyncify unwind inside SDL.
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_ASYNCIFY, "0");
    // Audio is optional and initialized by FSOUND_Init/Mix_OpenAudio. Do not
    // abort graphics if Safari or the host cannot create an audio context.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) == -1) {
        throw std::runtime_error(std::string("Can't initialize SDL") +
                                 SDL_GetError());
    }
    // Keyboard/touch play does not depend on Safari exposing the Gamepad API.
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == -1) {
        debug << "Optional gamepad input unavailable: " << SDL_GetError() << "\n";
    }
}

void Graphics::ToggleFullscreen() { SetGfxMode(x_, y_, d_, !fullscreen_); }
void Graphics::SetGfxMode(int x, int y, int d, bool fullscreen) {
    x_ = x;
    y_ = y;
    d_ = d;
    fullscreen_ = fullscreen;
    frame_texture_.reset();
    renderer_.reset();
    window_.reset();
    window_.reset(SDL_ErrWrap(SDL_CreateWindow(
        "Blip&Blop",
        x,
        y,
        x,
        y,
        SDL_WINDOW_SHOWN | (fullscreen * SDL_WINDOW_FULLSCREEN))));

    renderer_.reset(SDL_ErrWrap(SDL_CreateRenderer(
        window_.get(),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)));
}

SDL::Surface* Graphics::CreatePrimary() {
    /**/
    debug << "CreatePrimary() - Creating a 640 x 480 Surface"
          << "\n";
    return CreateSurface(640, 480, 0);
    // return 0;
}

SDL::Surface* Graphics::CreatePrimary(SDL::Surface*& back) {
    debug << "Graphics::CreatePrimary(SDL::Surface * & back) - Creating a "
             "640x480 surface"
          << "\n";
    SDL::Surface* tmp = CreateSurface(640, 480);
    back = CreateSurface(640, 480);
    tmp->SetBackBuffer(back);
    return tmp;
    // return 0;
}

SDL::Surface* Graphics::CreateSurface(int x, int y) {
    return CreateSurface(x, y, 0);
}

SDL::Surface* Graphics::CreateSurface(int x, int y, int flags) {
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    SDL_Surface* surf =
        SDL_CreateRGBSurface(0, x, y, 32, rmask, gmask, bmask, amask);

    SDL::Surface* tmp = new SDL::Surface(surf);
    tmp->FillRect(0, 0xFF000000);
    return tmp;
}

SDL_Surface* Graphics::CreateSDLSurface(int x, int y) {
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    SDL_Surface* surf =
        SDL_CreateRGBSurface(0, x, y, 32, rmask, gmask, bmask, amask);
    return (surf);
}

SDL::Surface* Graphics::LoadBMP(char* file) { return this->LoadBMP(file, 0); }

SDL::Surface* Graphics::LoadBMP(char* file, int flags) {
    /*SDL_Surface *bmp = 0;
      bmp = SDL_LoadBMP(file);
      if (bmp == 0){
      std::cout << SDL_GetError() << std::endl;
      return 0;
      }
      SDL_Texture *tex = 0;
      tex = SDL_CreateTextureFromSurface(ren, bmp);
      SDL_FreeSurface(bmp);
      return new SDL::Surface(tex);*/
    SDL_Surface* bmp = 0;
    bmp = SDL_LoadBMP(file);
    if (bmp == 0) {
        std::cout << SDL_GetError() << std::endl;
        return 0;
    }
    return new SDL::Surface(bmp);
}

bool Graphics::SetColorKey(SDL::Surface* surf, Pixel rgb) {
    SDL_SetColorKey(surf->Get(),
                    SDL_TRUE,
                    SDL_MapRGB(surf->Get()->format,
                               (rgb & 0xFF),
                               ((rgb >> 8) & 0xFF),
                               ((rgb >> 16) & 0xFF)));
    // TODO: set color key
    return true;
}

void Graphics::Flip() {
    SDL_Surface* surface = backSurface->Get();
    if (!frame_texture_) {
        frame_texture_.reset(SDL_ErrWrap(SDL_CreateTexture(
            renderer_.get(), surface->format->format,
            SDL_TEXTUREACCESS_STREAMING, surface->w, surface->h)));
        SDL_SetTextureBlendMode(frame_texture_.get(), SDL_BLENDMODE_NONE);
    }
    void* pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(frame_texture_.get(), nullptr, &pixels, &pitch) != 0) {
        throw std::runtime_error(SDL_GetError());
    }
    const int row_bytes = surface->w * surface->format->BytesPerPixel;
    for (int y = 0; y < surface->h; ++y) {
        std::memcpy(static_cast<Uint8*>(pixels) + y * pitch,
                    static_cast<Uint8*>(surface->pixels) + y * surface->pitch,
                    row_bytes);
    }
    SDL_UnlockTexture(frame_texture_.get());
    SDL_RenderClear(renderer_.get());
    SDL_RenderCopy(renderer_.get(), frame_texture_.get(), NULL, NULL);
    SDL_RenderPresent(renderer_.get());

    emscripten_sleep(1);
    // SDL_Delay(1);

    // SDL_SaveBMP(backSurface->Get(), "test/draw.bmp");
}

void Graphics::FlipV() {
    Flip();
    // SDL_RenderPresent(renderer);
}

void Graphics::Clear(int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer_.get(), r, g, b, 255);
    SDL_RenderClear(renderer_.get());
    SDL_RenderPresent(renderer_.get());
}

void Graphics::Clear(int c) {
    int r = (c >> 16) & 0xFF;
    int g = (c >> 8) & 0xFF;
    int b = (c >> 0) & 0xFF;

    SDL_SetRenderDrawColor(renderer_.get(), r, g, b, 255);
    SDL_RenderClear(renderer_.get());
    SDL_RenderPresent(renderer_.get());
}

void Graphics::Clear(RenderRect r2) {
    int r = (r2.dwFillColor >> 16) & 0xFF;
    int g = (r2.dwFillColor >> 8) & 0xFF;
    int b = (r2.dwFillColor >> 0) & 0xFF;

    SDL_Rect rect;
    rect.x = r2.left;
    rect.y = r2.top;
    rect.w = r2.right - r2.left;
    rect.h = r2.bottom - r2.top;

    SDL_SetRenderDrawColor(renderer_.get(), r, g, b, 255);
    SDL_RenderDrawRect(renderer_.get(), &rect);
}
