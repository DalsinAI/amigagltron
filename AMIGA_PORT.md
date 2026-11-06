GLtron 0.70 for AmigaOS 3.x
===========================

GLtron, the 3D lightcycle game (Andreas Umbach and others,
http://www.gltron.org), for AmigaOS 3.x on a 68040 or 68060 with an FPU and
an RTG screen, on OpenGPU's SDL 1.2 (sdl12-compat on SDL 2) and OpenGL.

Base: gltron-0.70-source.tar.gz from SourceForge (gltron-source/0.70,
sha256 e0c8ebb41a18a1f8d7302a9c2cb466f5b1dd63e9a9966c769075e6b6bdad8bb0).
Branch team2/gltron-amiga.

Status: builds; not yet run in the lab.


What you need
-------------

- A 68040 or 68060 with an FPU, AmigaOS 3.x, an RTG card (or the
  AmigaChrome lab).
- OpenGPU 0.6 or later: opengpu.library and LIBS:OpenGPU/SDL2.module (3 or
  later, for OpenGL) and GL.module (installed by OpenUp). GLtron draws only
  with OpenGL: on AmigaChrome GL runs on the GPU; elsewhere it is Mesa's
  softpipe on the 68k, which is slow.


How to play
-----------

Open the GLtron icon, or run GLtron from a Shell. The game finds its data
in its drawer (data, art, scripts) and keeps its settings in prefs/ and its
screenshots in snapshots/ there.

Steer with the keys GLtron's Configure menu shows (player 1: A and S by
default), Space pauses, Escape goes back to the menu. Music: the 0.70 source
has none; GLtron plays the WAV sound effects.


Building
--------

    make -f amiga/Makefile.amigaos -j2 dist

with GFOpenAmigaGCC 16.2.0-open4, the OpenNDK headers, the OpenGPU kit and
the Team's static libpng and zlib (see the Makefile's head). The drawer is
dist/GLtron (and dist/GLtron.info).


What was changed
----------------

- src/gltron.c: main() first moves into the drawer (amiga/amiga_start.c),
  and closes at exit the screens SDL2.module 0.6 can leave
  (amiga/amiga_screens.c). amiga/stack.c: 512 KB of stack.
- src/filesystem/dirsetup.c: no search of $PATH for the program on the
  Amiga (it is in its drawer already). The Makefile gives the drawer's
  prefs and snapshots as the settings and screenshot directories (upstream:
  the home directory).
- nebu/scripting/scripting.c: scripting_RunFile, scripting_Run and
  scripting_RunFormat take const char *, as their header declares them
  (GCC 16 refuses the mismatch).
- amiga/sound_lite.c, amiga/include/SDL_sound.h: SDL_sound's calls that
  GLtron makes, for WAV files, on SDL's SDL_LoadWAV_RW and SDL_ConvertAudio
  (there is no SDL_sound for the Amiga's SDL here).
- No configure: amiga/Makefile.amigaos has the sources and definitions.


Licences
--------

GLtron: GPL 2 (COPYING). Lua 5.0 (lua/): MIT. The Amiga files (amiga/): GPL
2 or later, the AmiCradle Team.
