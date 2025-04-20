#include "Define.h"
#include "SDL.h"

int SCREEN_WIDTH = 480;
int SCREEN_HEIDHT = 800;

void InitDisplaySize() {
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    SCREEN_WIDTH = DM.w;
    SCREEN_HEIDHT = DM.h;
}