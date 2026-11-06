/* GLtron for AmigaOS: what main() does first.
 * The game reads its data and keeps its saves (prefs/, snapshots/) in the current
 * directory, so the program moves into its drawer, as Workbench does.
 * Then the screens OpenGPU's SDL2.module may leave are dealt with at exit
 * (amiga/amiga_screens.c).
 * The AmiCradle Team. GPL 2 or later, like GLtron. */
#include <stdlib.h>
#include <proto/dos.h>

void amiga_screens_snapshot(void);
void amiga_screens_close_leftovers(void);

static BPTR old_dir, prog_dir;

static void amiga_start_end(void)
{
	if (prog_dir) {
		CurrentDir(old_dir);
		UnLock(prog_dir);
		prog_dir = 0;
	}
}

void amiga_start(void)
{
	BPTR here = GetProgramDir();
	if (here && (prog_dir = DupLock(here)) != 0) {
		old_dir = CurrentDir(prog_dir);
		atexit(amiga_start_end);
	}
	amiga_screens_snapshot();
	atexit(amiga_screens_close_leftovers);	/* runs after SDL_Quit */
}
