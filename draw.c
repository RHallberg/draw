#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *canvas = NULL;

typedef struct {
    int x;
    int y;
} MousePos;
MousePos mpos = {0, 0};
bool draw = false;
bool clear = false;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Simple drawing app", "1.0", "com.example.drawing");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("draw", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    int ww;
    int wh;
    SDL_GetWindowSize(window, &ww, &wh);
    canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ww, wh);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // quit
    if (event->type == SDL_EVENT_QUIT || (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_Q)) {
        return SDL_APP_SUCCESS;
    }
    else if(event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_C){
      clear = true;
    }
    else if(event->type == SDL_EVENT_KEY_UP && event->key.key == SDLK_C){
      clear = false;
    }
    else if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == 1 && event->button.down){
      draw = true;
    }
    else if(event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == 1 && !event->button.down){
      draw = false;
    }
    else if(event->type == SDL_EVENT_MOUSE_MOTION){
      mpos.x = event->motion.x;
      mpos.y = event->motion.y;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{

    SDL_SetRenderTarget(renderer, canvas);
    if(clear){
      SDL_RenderClear(renderer);
    }
    if(draw){
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderPoint(renderer, mpos.x, mpos.y);
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, canvas, NULL, NULL);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{

}
