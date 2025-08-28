#define SDL_MAIN_USE_CALLBACKS 1
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define S_UNDO 10

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *canvas = NULL;

typedef struct {
    int x;
    int y;
} Point;

typedef struct ListElem {
  Point p1;
  Point p2;
  struct ListElem *next;
} ListElem;

// Ring buffer for undo-actions
int i_undo = 0;
ListElem *undobuf[S_UNDO];

Point mpos = {0, 0};
Point prevmpos = {0, 0};
bool draw = false;
bool clear = false;
bool undo = false;

ListElem* create_node(Point p1, Point p2);

void add_node(Point p1, Point p2);

void incr_iundo(void);
void decr_iundo(void);
void free_undo(int i);

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

    // Null instantiate the undo buffer
    for(int i = 0; i < S_UNDO; i++){
      undobuf[i] = NULL;
    }

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
    else if(event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_U){
      undo = true;
    }
    else if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == 1 && event->button.down){
      draw = true;
    }
    else if(event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == 1 && !event->button.down){
      draw = false;
      incr_iundo();
    }
    else if(event->type == SDL_EVENT_MOUSE_MOTION){
      prevmpos.x = mpos.x;
      prevmpos.y = mpos.y;
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
      clear = false;
    }
    if(draw){
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderLine(renderer, prevmpos.x, prevmpos.y, mpos.x, mpos.y);
      add_node(prevmpos, mpos);
    }
    if(undo){
      i_undo = (i_undo - 1 + S_UNDO) % S_UNDO;
      if(undobuf[i_undo] != NULL){
        ListElem *cur_elem = undobuf[i_undo];
        while(cur_elem != NULL && cur_elem->next != NULL){
          ListElem *freeme = cur_elem;
          cur_elem = cur_elem->next;
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
          SDL_RenderLine(renderer, freeme->p1.x, freeme->p1.y, freeme->p2.x, freeme->p2.y);
          free(freeme);
        }
        undobuf[i_undo] = NULL;
      }
      undo = false;
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

void add_node(Point p1, Point p2)
{
  ListElem *n = create_node(p1,p2);
  if(undobuf[i_undo] == NULL){
    undobuf[i_undo] = n;
  }
  else{
    ListElem *h = undobuf[i_undo];
    n->next = h;
    undobuf[i_undo] = n;
  }
}

ListElem* create_node(Point p1, Point p2)
{
    ListElem *node = malloc(sizeof(ListElem));
    node->p1 = p1;
    node->p2 = p2;
    node->next = NULL;
    return node;
}

void incr_iundo(void)
{
  i_undo = (i_undo + 1) % S_UNDO;
  if(undobuf[i_undo] != NULL){
    free_undo(i_undo);
  }
}

void free_undo(int i)
{
  ListElem *cur_elem = undobuf[i];
  while(cur_elem->next != NULL){
    ListElem *freeme = cur_elem;
    cur_elem = cur_elem->next;
    free(freeme);
  }
  undobuf[i] = NULL;
}
