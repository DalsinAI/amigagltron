GLtron 0.70 for AmigaOS 3.x
===========================

GLtron, the 3D lightcycle game (Andreas Umbach and others,
http://www.gltron.org), for AmigaOS 3.x on a 68040 or 68060 with an FPU and
an RTG screen, on OpenGPU's SDL 1.2 (sdl12-compat on SDL 2) and OpenGL.

Base: gltron-0.70-source.tar.gz from SourceForge (gltron-source/0.70,
sha256 e0c8ebb41a18a1f8d7302a9c2cb466f5b1dd63e9a9966c769075e6b6bdad8bb0).
Branch team2/gltron-amiga.

Status: plays in the lab (Instance-45, 6 November 2026, SDL2.module from
gfopenrtg 29d32bf, GL on the 68k's softpipe): the menus, a game against
the computer's three players (steering, trails, crashes, the round's
winner) and Quit with the credits; settings saved in prefs/gltron.ini.


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
in its drawer (data, art, scripts) and keeps its settings in
prefs/gltron.ini and its screenshots in snapshots/ there.

Steer with the keys GLtron's Configure menu shows (player 1: A and S by
default), Space pauses (a game starts paused), Escape goes back to the
menu. Music: the 0.70 source has none, and the game plays without it; the
sound effects are there. Shadows are solid black, as upstream draws them
without a stencil buffer (settings.use_stencil = 0, its default).



Standard keys
-------------

Every AmiCradle game has the same keys on Right Amiga (GFOpenUp's
SDL2.module, after 0.8.26):

- Right Amiga + F: window or full screen, remembered for next time
- Right Amiga + P: pause (Right Amiga + P again goes on)
- Right Amiga + Q: quit
- Right Amiga + M: free the mouse pointer, or let the game take it again
- Right Amiga + S: a screenshot in the drawer's Screenshots
- Help: this game's keys (the drawer's Controls file)
- Right Amiga + F1, F2: F11, F12; Right Amiga + cursor keys: Page Up,
  Page Down, Home, End

The FULLSCREEN tooltype on the game's icon (take the brackets off) starts
it full screen.


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
- src/include/game/gltron.h: the settings file is gltron.ini (as on
  Windows), not .gltronrc.
- src/audio/sound.c, src/audio/sound_glue.cpp: no music files, or no
  song, means no music (upstream stops the game); the music calls check
  that there is a song.
- src/filesystem/dirsetup.c: no search of $PATH for the program on the
  Amiga (it is in its drawer already). The Makefile gives the drawer's
  prefs and snapshots as the settings and screenshot directories (upstream:
  the home directory).
- nebu/scripting/scripting.c: scripting_RunFile, scripting_Run and
  scripting_RunFormat take const char *, as their header declares them
  (GCC 16 refuses the mismatch).
- amiga/sound_lite.c, amiga/include/SDL_sound.h: SDL_sound's calls that
  GLtron makes, for PCM WAV files (there is no SDL_sound for the Amiga's
  SDL here). nebu/audio/SourceSample.cpp and SourceMusic.cpp open the files
  through it (Sound_NewSampleFromFile), which reads and converts them
  itself: sdl12-compat's SDL_LoadWAV_RW does not come back on OpenGPU.
- No configure: amiga/Makefile.amigaos has the sources and definitions,
  and links sdl12-compat's GL table first (-lSDL_gl before -lSDL, as
  sdl-config --gl does).


Licences
--------

GLtron: GPL 2 (COPYING). Lua 5.0 (lua/): MIT. The Amiga files (amiga/): GPL
2 or later, the AmiCradle Team.
