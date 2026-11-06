/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/
#include "SDL.h"

#include "game/init.h"
#include "filesystem/path.h"
#include "base/util.h"
#include "SDL.h"

#ifdef __amigaos__
void amiga_start(void); /* amiga/amiga_start.c: into the drawer */
#endif

int main(int argc, char *argv[] ) {
#ifdef __amigaos__
	amiga_start();
#endif
	initSubsystems(argc, argv);
	runScript(PATH_SCRIPTS, "main.lua");
  return 0;
}







