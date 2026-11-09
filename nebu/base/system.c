#include "base/nebu_system.h"

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

Callbacks *current = 0;
static int return_code = -1;
static int redisplay = 0;

void SystemExit() {
  fprintf(stderr, "[system] shutting down SDL now\n");
  SDL_Quit();
  fprintf(stderr, "[system] exiting application\n");
}

unsigned int SystemGetElapsedTime() {
  /* fprintf(stderr, "%d\n", SDL_GetTicks()); */
  return SDL_GetTicks();
}

int SystemMainLoop() {
  SDL_Event event;
  
	return_code = -1;
  while(return_code == -1) {
    while(SDL_PollEvent(&event) && current) {
			switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_JOYAXISMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION:
				SystemHandleInput(&event);
				break;
			case SDL_QUIT:
				/* SDL is gone after SystemExit(): end here, as the credits'
				   quit does, or the loop keeps drawing with no SDL and the
				   program never returns to the Shell. */
				SystemExit();
				exit(0);
			default:
				/* ignore event */
				break;
      }
    }
    if(redisplay) {
      current->display();
      redisplay = 0;
    } else
      current->idle();
  }
	if(current->exit)
		(current->exit)();
	return return_code;
}
  
void SystemRegisterCallbacks(Callbacks *cb) {
  current = cb;
}

void SystemExitLoop(int value) {
	return_code = value;
}

void SystemPostRedisplay() {
  redisplay = 1;
}

